# Tutorial: Create a Windows Machine Learning application with Python/WinRT

This tutorial is a port of the [WinML C++/WinRT tutorial](https://docs.microsoft.com/en-us/windows/ai/get-started-desktop) to Python. Please review the original documentation for background on WinML. This document will only focus on the differences related to using Python/WinRT instead of C++/WinRT.

> Note, xlang in general and Python/WinRT in particular is very, very early in its development. It is not ready for general usage building Python applications against Windows Runtime APIs. Many features are missing or will be changed while development continues. Only the core scenario described below has been tested end-to-end. You are encouraged to experiment outside the bounds of the core scenario described in this document. Just remember that Python/WinRT is in active development as you experiment and please [open issues](https://github.com/Microsoft/xlang/issues) when (not if!) you find bugs or missing functionality.

## Prerequisites

* [Visual Studio 2017](https://developer.microsoft.com/windows/downloads), version 15.8 or later.
  * This tutorial was tested against Visual Studio 15.8.7, the latest version as of this writing.
  * Visual Studio's Desktop Development with C++ workload installation is required.
* [Windows 10](https://developer.microsoft.com/windows/downloads), version 1809 or later.
* [Windows SDK](https://www.microsoft.com/software-download/windowsinsiderpreviewSDK), build 17763 or later
  * Windows SDK, build 17763 can be installed as part of Visual Studio 15.8.7.
* [Python for Windows](https://www.python.org), version 3.6 or later
  * Visual Studio's Python Development workload is _not_ required, but does include Python 3.6.
  * This tutorial is written assuming Python 3.6 is available in the VS 2017 Developer Command Prompt.

## Compile the Python/WinRT Generated Code

The pywinrt_output folder contains code for a native Python extension module, generated by the Python/WinRT tool from the [xlang project](https://github.com/Microsoft/xlang). This extension module enables Python code to access to a subset of the [Windows Runtime APIs](https://docs.microsoft.com/en-us/uwp/api/) available in Windows 10, version 1809.

> At this time, the Python/WinRT tool is only distributed as source code. This sample includes the Python native extension module  generated by Python/WinRT to enable the reader to complete the tutorial without needing to build the Python/WinRT tool. A [seperate document](readme-advanced.md) details how this code was generated for those wishing to experiment futher with Python/WinRT in its current state.

To build the Python native extension module, open a VS 2017 developer command prompt, change to the /pywinrt_output directory and execute the following command:

``` shell
C:\Users\user\Source\xlang\samples\python\winml_tutorial\pywinrt_output>py setup.py build
```

This step will build the extension module using [Python's setuptools package](https://github.com/pypa/setuptools). The resulting compiled module will be named `_pyrt.cp36-win_amd64.pyd` and will be available in the `/pywinrt_output/build/lib.win-amd64-3.6/` directory.

## Create the Python file

Create a new python file in the sample root folder (i.e. in the same folder that contains the /python_output folder) and open it with your favorite text editor. The name of the python file doesn't matter, but this tutorial will assume the file is named winml_tutorial.py

> Note, [complete_winml_tutorial.py](complete_winml_tutorial.py) contains the complete code for this tutorial, if you'd rather not type the code in yourself.

## Add Generated Python/WinRT Extension Module to sys.path

First, we are going to add code to enable Python to locate the extension module we compiled in the Compile the Python/WinRT Generated Code step. The following code updates [sys.path](https://docs.python.org/3/library/sys.html#sys.path) with the folder containing the compiled extension module.

> Typically, Python extension modules are be installed into Python's site-packages folder by setuptools. Installed extension modules are available to Python without additional effort. However, given the pre-alpha nature of Python/WinRT, Step 1 does not install the generated native extension module. So for now, we need to tell the Python interpreter where to find the extension module.

``` python
import sys
vi = sys.version_info
dirname = "lib.{2}-{0}.{1}".format(vi.major, vi.minor, "win-amd64" if sys.maxsize > 2**32 else "win32")
test_module_path = "./pywinrt_output/build/" + dirname
sys.path.append(str(test_module_path))
```

## Load the WinML Model

Now that the Python/WinRT extension module has been added to the Python interpreter's path, it can be used from Python directly. We will use the [LearningModel.LoadFromFilePath](https://docs.microsoft.com/uwp/api/windows.ai.machinelearning.learningmodel.loadfromfilepath) API to load the ONNX model from disk, as per the [load the model](https://docs.microsoft.com/en-us/windows/ai/get-started-desktop#load-the-model) step from the original tutorial.

First, instead of manually adding timing code to every function like the C++/WinRT version does, let's add a [Python decorator](https://docs.python.org/3.7/glossary.html#term-decorator) so we only have to write the timing code once.

``` python
def timed_op(fun):
    import time

    def wrapper(*args, **kwds):
        print("Starting", fun.__name__)
        start = time.perf_counter()

        ret = fun(*args, **kwds)

        end = time.perf_counter()
        print(fun.__name__, "took", end - start, "seconds")
        return ret

    return wrapper
```

With the timed_op decorator, LoadModel in Python is a trivial, one-line function. We simply need to import _pyrt extension module and then we can access the LoadFromFilePath static function directly from the LearningModel type.

> Note, Python/WinRT currently flattens WinRT namespaces. WinRT namespace support will come in a future update.

``` python
import _pyrt

@timed_op
def load_model(model_path):
    return _pyrt.LearningModel.LoadFromFilePath(model_path)
```

We then call the load_model function from the main part of the python script, passing the path to the provided ONNX model. Note the call to init_apartment, the Python projection of [RoInitialize](https://docs.microsoft.com/en-us/windows/desktop/api/roapi/nf-roapi-roinitialize). Most WinRT APIs require WinRT to be initialized in order to work correctly.

``` python
import os.path

_pyrt.init_apartment()
model = load_model(os.path.abspath("./winml_content/SqueezeNet.onnx"))
```

Running this code from Python now should result in something similar to the following (the time it takes to load the model will vary).

``` shell
C:\Users\user\Source\xlang\samples\python\winml_tutorial>py winml_tutorial.py

Starting load_model
load_model took 0.7060564 seconds
```

## Load the Image to Evaluate

Next, we'll [load an image](https://docs.microsoft.com/en-us/windows/ai/get-started-desktop#load-the-image) that we are going to evaluate with the loaded model.

This step requires the use of async WinRT methods. Automatic projection of WinRT async methods as a [Python awaitable](https://docs.python.org/3.7/glossary.html#term-awaitable) is not complete, so we need some helper code to do this conversion.

``` python
import _pyrt
import asyncio

def timed_op(fun):
    import time

    def sync_wrapper(*args, **kwds):
        print("Starting", fun.__name__)
        start = time.perf_counter()

        ret = fun(*args, **kwds)

        end = time.perf_counter()
        print(fun.__name__, "took", end - start, "seconds")
        return ret

    async def async_wrapper(*args, **kwds):
        print("Starting", fun.__name__)
        start = time.perf_counter()

        ret = await fun(*args, **kwds)

        end = time.perf_counter()
        print(fun.__name__, "took", end - start, "seconds")
        return ret

    return async_wrapper if asyncio.iscoroutinefunction(fun) else sync_wrapper

async def wrap_async_op(op):
    loop = asyncio.get_event_loop()
    future = loop.create_future()

    def callback(operation, status):
        if status == 1:
            result = operation.GetResults()
            loop.call_soon_threadsafe(asyncio.Future.set_result, future, result)
        elif status == 2:
            loop.call_soon_threadsafe(asyncio.Future.set_exception, future, asyncio.CancelledError())
        elif status == 3:
            loop.call_soon_threadsafe(asyncio.Future.set_exception, future, RuntimeError("AsyncOp failed"))
        else:
            loop.call_soon_threadsafe(asyncio.Future.set_exception, future, RuntimeError("Unexpected AsyncStatus"))

    op.put_Completed(callback)

    return await future

def run_async_code(code):
    loop = asyncio.get_event_loop()
    loop.run_until_complete(code())
    loop.close()
```

The first function in the previous code block is an updated version of the timed_op decorator. This version can async functions as well as normal ones.

The second function, wrap_async_op, converts a WinRT IAsyncOperation into a Python awaitable using [asyncio.Future](https://docs.python.org/3.7/library/asyncio-future.html).

The final function, run_async_code, enables syncronous code (such as our main block of program execution) to call an async function.

With these helpers now in place, we can write async python code to load an image and convert it into a [VideoFrame](https://docs.microsoft.com/en-us/uwp/api/Windows.Media.VideoFrame) for use with the LearningModel we created earlier.

> Note, WinRT enumerations are currently projected as integers in Python. Full Python enumeration support will come in a future update to Python/WinRT.

``` python
@timed_op
async def load_image_file(file_path):
    file = await wrap_async_op(_pyrt.StorageFile.GetFileFromPathAsync(file_path))
    stream = await wrap_async_op(file.OpenAsync(0)) # 0 == FileAccessMode::Read
    decoder = await wrap_async_op(_pyrt.BitmapDecoder.CreateAsync(stream))
    software_bitmap = await wrap_async_op(decoder.GetSoftwareBitmapAsync())
    return _pyrt.VideoFrame.CreateWithSoftwareBitmap(software_bitmap)
```

In order to intermix syncronous and asyncronous code, we will move our main block of program exectuion into an async function, which we can then call from a syncronous context using run_async_code:

``` python
async def async_main():
    import os.path

    model_path = os.path.abspath("./winml_content/SqueezeNet.onnx")
    model = load_model(model_path)

    image_file = os.path.abspath("./winml_content/kitten_224.png")
    image_frame = await load_image_file(image_file)

_pyrt.init_apartment()
run_async_code(async_main)
```

Running this code from Python now should result in something similar to the following (again, timings will vary).

``` shell
Starting load_model
load_model took 0.7183248 seconds
Starting load_image_file
load_image_file took 0.17086919999999994 seconds
```

## Bind Input and Output

Now that the model and image to be evaluated are loaded, we create a LearningModelSession to [bind them together](https://docs.microsoft.com/en-us/windows/ai/get-started-desktop#bind-the-input-and-output).

``` python
@timed_op
def bind_model(model, image_frame):
    device = _pyrt.LearningModelDevice(0) # 0 == LearningModelDeviceKind::Default
    session = _pyrt.LearningModelSession(model, device)
    binding = _pyrt.LearningModelBinding(session)
    image_feature_value = _pyrt.ImageFeatureValue.CreateFromVideoFrame(image_frame)
    binding.Bind("data_0", image_feature_value)
    shape = _pyrt.TensorFloat.Create([1, 1000, 1, 1])
    binding.Bind("softmaxout_1", shape)
    return (session, binding)
```

Instead of using global variables like the C++/WinRT version, this Python code is passing all state as parameters and return values. In bind_model, we need to return two values - the session and the binding - so we group them into a tuple.

We can simply add the bind_model call to our existing async_main function, using destructuring assignment to assign the grouped return values into seperate variables.

``` python
async def async_main():
    import os.path

    model_path = os.path.abspath("./winml_content/SqueezeNet.onnx")
    model = load_model(model_path)

    image_file = os.path.abspath("./winml_content/kitten_224.png")
    image_frame = await load_image_file(image_file)

    session, binding = bind_model(model, image_frame)
```

Running this code with Python now should result in something similar to the following.

``` shell
Starting load_model
load_model took 0.7168329999999999 seconds
Starting load_image_file
load_image_file took 0.1684 seconds
Starting bind_model
bind_model took 0.020646799999999965 seconds
```

## Evaluate the Model

We are now ready to [evaluate the model](https://docs.microsoft.com/en-us/windows/ai/get-started-desktop#evaluate-the-model) and find out what the image represents.

> Note, WinRT properties are currently projected as get_/put_ prefixed methods in Python. Full Python property support will come in a future update to Python/WinRT.

``` python
@timed_op
def evaluate_model(session, binding):
    results = session.Evaluate(binding, "RunId")
    o = results.get_Outputs().Lookup("softmaxout_1")
    result_tensor = _pyrt.TensorFloat._from(o)
    return result_tensor.GetAsVectorView()
```

The [Evaluate method](https://docs.microsoft.com/en-us/uwp/api/windows.ai.machinelearning.learningmodelsession.evaluate) returns its [Outputs](https://docs.microsoft.com/en-us/uwp/api/windows.ai.machinelearning.learningmodelevaluationresult.outputs) as a string to object IMap.

While Python types are typically dynamically typed (sometimes know as "duck typing"), WinRT types are statically typed. Thus the need to convert the softmaxout_1 output object to the correct type in order for Python work with it. To convert a WinRT base object to a different static type, all WinRT classes and interfaces suport [QueryInterface](https://docs.microsoft.com/en-us/windows/desktop/api/unknwn/nf-unknwn-iunknown-queryinterface(q_)). In Python, QueryInterface is  projected as a _from static method on the type we want to convert to. All WinRT classes and non-parameterized interfaces expose a _from method.

Adding model evaluation to async_main is a simple one line addition:

``` python
async def async_main():
    import os.path

    model_path = os.path.abspath("./winml_content/SqueezeNet.onnx")
    model = load_model(model_path)

    image_file = os.path.abspath("./winml_content/kitten_224.png")
    image_frame = await load_image_file(image_file)

    session, binding = bind_model(model, image_frame)
    results = evaluate_model(session, binding)
```

Running this code with Python now should result in something similar to the following.

``` shell
Starting load_model
load_model took 0.7029269 seconds
Starting load_image_file
load_image_file took 0.15494339999999995 seconds
Starting bind_model
bind_model took 0.016178600000000043 seconds
Starting evaluate_model
evaluate_model took 0.022036400000000067 seconds
```

## Print the Results

Finally, we need to print the model evaluation results. The possible model results are stored in a comma-seperated file named Labels.txt. Loading this data in Python is straightforward:

``` python
def load_labels(labels_path):
    import csv
    labels = dict()
    with open(labels_path) as labels_file:
        labels_reader = csv.reader(labels_file)
        for label in labels_reader:
            label_text = ', '.join(label[1:])
            labels[int(label[0])] = ', '.join(label[1:])
    return labels
```

`load_labels` returns a Python dictionary mapping model results to a text string. All that remains now is to loop thru the results, looking for the top three probabilities and then print them out.

``` python
def print_results(results, labels):
    topProbabilities = [0.0 for x in range(3)]
    topProbabilityLabelIndexes = [0 for x in range(3)]

    for i in range(results.get_Size()):
        for j in range(3):
            result = results.GetAt(i)
            if result > topProbabilities[j]:
                topProbabilityLabelIndexes[j] = i
                topProbabilities[j] = result
                break

    print()
    for i in range(3):
        print(labels[topProbabilityLabelIndexes[i]], "with confidence of", topProbabilities[i])

async def async_main():
    import os.path

    model_path = os.path.abspath("./winml_content/SqueezeNet.onnx")
    model = load_model(model_path)

    image_file = os.path.abspath("./winml_content/kitten_224.png")
    image_frame = await load_image_file(image_file)

    session, binding = bind_model(model, image_frame)
    results = evaluate_model(session, binding)

    labels_path = os.path.abspath("./winml_content/Labels.txt")
    labels = load_labels(labels_path)

    print_results(results, labels)
```

Executing this code with Python one final time should reveal the results of the model evaluation, predicting that the image is a tabby cat.

``` shell
Starting load_model
load_model took 0.7041156000000001 seconds
Starting load_image_file
load_image_file took 0.1883804 seconds
Starting bind_model
bind_model took 0.01686500000000002 seconds
Starting evaluate_model
evaluate_model took 0.021589099999999917 seconds

tabby,  tabby cat with confidence of 0.931460976600647
Egyptian cat with confidence of 0.06530658155679703
Persian cat with confidence of 0.00019303907174617052
```