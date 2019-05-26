# PVG

A graphical replacement to the popular pv utility on linux. Designed to be drop-in (apart from arguments) will display a curses graph of the data rate going through a pipe.

## Installation
### Building from source
This project has no external dependencies aside from the standard autotools build tools. You can build from source with the following procedure:

```
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
```

You will need the following libraries:

* libncurses

### Installing pre-build binary

You can find pre-built binaries for various linux and unix flavours under the releases page. You can install by downloading one of the tarballs and copying into your preferred bin folder.

### Installing from package

Currently there are no standard packages available. This is something I am actively working on but is not ready yet. Sorry!

## Usage

The usage is very simple. There are currently no command-line options so where-ever you would normally use `pv` just use `pvg` instead and you should be greeted with a nice pretty graph. If this is ever not the case, please open an issue.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[MIT](https://choosealicense.com/licenses/mit/)
