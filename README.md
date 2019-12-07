# OCR
Optical character recognition program based on deep learning.

## How to use

### Graphical User Interface

The character recognition program can be used through a GUI. Use `./ocr` to launch it. Drag and drop an image in the GUI to process it. The extracted text is shown on the left side. The result can then be saved into a txt file.

### Command Line Interface

The CLI offers more functionalities. Type `./ocr help` to list available commands. Type `./ocr {command}` to see the usage of the command.
- `./ocr write_dataset`: Creates a new learning dataset from a directory containing images to be segmented. Each image must contain the following string : `0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'()-_.,?!:;`.
- `./ocr learn`: Trains a new neural network from a given dataset. The so created network can be saved.
- `./ocr read`: Reads an image using the given neural network. The extracted text is displayed and saved into a txt file.
