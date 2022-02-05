# Paint-using-Qt
Paint is a simple tool for draw images.

## Installation

### Windows

*Now avaiable in releases*

### Linux

In Releas you will find a executable for x86_64/arm64

### Mac/iPhone

I don’t own a Mac, so I cant make an executable for it. For the time being Mac users will have to compile the application from source.

### Android

Take a look at [releases](https://github.com/software-made-easy/Paint-using-Qt/releases/latest).

### Build from source

> clone the repo using `git clone https://github.com/software-made-easy/Paint-using-Qt && cd Paint-using-Qt.git`

> *For Android please clone [this](https://gitlab.com/mattbas/Qt-Color-Widgets) repo from gitlab. using `git clone https://gitlab.com/mattbas/Qt-Color-Widgets.git`*

> create build direction by enter this command: `mkdir build && cd build`
> create make file: `qmake ../Paint.pro`
> build: `make`
> wait

Full script:
```
git clone https://github.com/software-made-easy/Paint-using-Qt && cd Paint-using-Qt.git
git clone https://gitlab.com/mattbas/Qt-Color-Widgets.git
mkdir build && cd build
qmake ../Paint.pro
make
```

## Credits
> [mattbas](https://gitlab.com/mattbas)
