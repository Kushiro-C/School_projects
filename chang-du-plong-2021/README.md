# chang-du-plong-2021

Patrick Chang 71802046

Vincent Du    71801856

---

## Prerequisite

You need to have java, arduino and PostGreSQL on your machine.

Install Arduino with this command : sudo apt-get install arduino

cable the arduino as shown in the picture named cable
use the Arduino program and transfer the program called sketch_arduino.ino
to the arduino


The first launch will fill the database with the musics,
but it might take a long time (around 10 minutes).

When the database is filled, you can launch the app again and enjoy.


## Build and launch the project with gradle

### Gradle Version

gradle version is 7.3.3


### Build the project

`./gradlew build`


### Launch the project

`./gradlew run`


### Launch the Tests

`./gradlew test`


### Clean the build

`./gradlew clean`

---

### If the gradle build does not work, you might need to do :

`gradle wrapper --gradle-version 7.3.3 --distribution-type all`

`./gradlew wrapper --gradle-version 7.3.3`


---
### Slides :

https://docs.google.com/presentation/d/1lb9setHNm3dBv6zQjIAaeJ0UIsLaGcH3fH_nLL1w-SU/edit?usp=sharing

### Things that might be useful

A guide on how to install gradle (pick latest 7.3.3 version)

https://linuxize.com/post/how-to-install-gradle-on-ubuntu-18-04/

