# Sample README file for C++ programming project

This is an initial git repository for your programming project.
It contains an initial directory structure that we wish you could
use as follows:

  * `plan/' -- Here is your project plan. You may use different sources,
    but we hope to get the pan **also as pdf file**.

  * `doc/` -- here are the documentation sources related to your project.
    You may use different tools for documentation (e.g. latex),
    but we hope to get the final version of the document
    **also as pdf file**.

  * `src/` -- here are the C++ source files. You can have subfolders as needed.

  * `README.md` -- This file that you should modify to represent
    your project.



## General guidelines
- C++11 standard!
- Indent is 4 spaces
- Variables are declared at the beginning of their scopes
- member variables as variables_
- Brackets on the next line, even in single row
```cpp
void even()
{ // <- newline
    //
    if(oneLineOnly)
    {
        stillBrackets();
    }
    else
    {
        //...
    }
}
```


## Documentation
- Use [Doxygen](http://www.doxygen.org)

## Install library
- FFmpeg
How to build FFmpeg from source
1. Because we want to also use ffserver, so we use the version ffmpeg 3.4
```
    git clone https://github.com/FFmpeg/FFmpeg.git
```
2. 
```
    ./configure --enable-shared
```
3. we might need "sudo apt-get install nasm"
4.
``` 
    make
```
5. 
```
    make install
```
6. ` For your convenience, in the folder ffmpeg, there are static libraries and include file in case the version you install is not working.`
- How to install FFmpeg from apt get
```
    sudo add-apt-repository ppa:jonathonf/ffmpeg-3
```
```
    sudo apt update && sudo apt install ffmpeg libav-tools x264 x265 libavdevice-dev
```

- sdl and sdl2 install
```
    sudo apt-get install libsdl1.2-dev
```
```
    sudo apt-get install libsdl2-dev
```
- gtest
1. git clone https://github.com/google/googletest.git
2. cd googletest
3. mkdir build
4. cd build
5. cmake ..
6. make
7. sudo make install


## Sample Video
- In folder sample video, there are two video for playing

