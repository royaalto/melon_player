# Project evaluation

**Name of the evaluated project:**
Media Player 5

**Score:**
24 out of 33 points

## 0. Project plan (0-3p): 1p

You can find the evaluation of the project plan from your git repo.

## 1. Overall design and functionality (0-6p): 5p

  * 1.1: The implementation corresponds to the selected topic and scope. The extent of project is large enough to accommodate work for everyone (2p)

The topic and scope of the project are alright.

  * 1.2: The software structure is appropriate, clear and well documented. e.g. class structure is justified, inheritance used where appropriate, information hiding is implemented as appropriate. (2p)

Overall program structure is quite clear and logical. Well commented source code.
Documentation seems a bit unfinished here and there. (-0.5)
While the readability is generally good, the source code is biased to a couple of files and the file structure itself is a bit messy (e.g. src/src/). (-0.5p)

  * 1.3: Use of external libraries is justified and well documented. (2p)

The used libraries' installation is instructed and their usage is described well enough.

## 2. Working practices (0-6p): 4p

  * 2.1: Git is used appropriately (e.g., commits are logical and frequent enough, commit logs are descriptive). (2 p)

Git is used mostly appropriately, though the commit messages are too vague. (-0.5p)

  * 2.2: Work is distributed and organised well. Everyone contributes to the project and has a relevant role that matches his/her skills. The distribution of roles is described well enough. (2p)

Work division is not documented completely. (-0.5p)
No work log. (-1p)

  * 2.3: Quality assurance is appropriate. Implementation is tested comprehensively and those testing principles are well documented. (2p)

Manual testing has been conducted and it's documented well. Also trials with unit tests.

## 3. Implementation aspects (0-8p): 6p

  * 3.1: Building the software is easy and well documented. CMake or such tool is highly recommended. (2p)

Instructions and the provided building tools make the building easy.

  * 3.2: Memory management is robust, well-organised and coherent. E.g., smart pointers are used where appropriate or RO3/5 is followed. The memory management practices should be documented. (2p)

It's hard to make sense about the memory management (ffmpeg hook for example) without documentation about the practices used. (-0.5p)
Heap is used (new/delete) without smart-pointers. (-0.5p)

  * 3.3: C++ standard library is used where appropriate. For example, containers are used instead of own solutions where it makes sense. (2 p)

While the standard library usage isn't hight, it isn't used inappropriately.

  * 3.4: Implementation works robustly also in exceptional situations. E.g., functions can survive invalid inputs and exception handling is used where appropriate. (2p)

Some error handling.
For some reason, program didn't perform as it did when it was demonstrated. There was no audio and some controls and features made the program to crash or abort. (-1p)

## 4. Project extensiveness (0-10p): 8p

  * Project contains features beyond the minimal requirements: Most of the projects list additional features which can be implemented for more points. Teams can also suggest their own custom features, though they have to be in the scope of the project and approved by the course assistant who is overseeing the project. (0-10p)

    - Audio browsing & loading from external directories (1p)
    - Displaying audio information by reading metadata (1p)
    - Modifying & saving audio information (tagging, rating, writing metadata) (1p)
    - Drag & drop support (1p)
    - Video support (4p)

    Most features are worth 1p because of Qt or the implementation's quality / misbehavior.

