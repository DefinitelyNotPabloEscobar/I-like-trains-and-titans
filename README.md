# Hello there!

So for start the project might take a bit to load in the beginning This because he will be generating the wood texture and loading all the meshes. 

## How to run?

The project will be delivered as a visual studio solution. To run, open the file
<ProjEXEC.sln> in the main folder with visual studio 2019. After this compile and run the project.


## Start

The user will start in the first person mode, by this I mean the camera will move according to your mouse movement, to move use **w,a,s,d**. You can **press shift_left** to run instead. You can also jump using **space**
Note: moving will take a consideration on where the user is looking.

## Hop on the train

To get in the train you can use **r** please note that it will only work if you are close to the train.
To leave the train press **r** again.

## In the train

Welcome aboard! You are in the train and now you will see the front camera train. To switch between camera use the **c** key. There are 4 cameras implemented:
 
 - The first one is the front view, you cant move her, but she will follow the train's movement.
 - The second, its a camera is always above the train.
 - The third will follow the train, however you can move around my using the your mouse and pressing the **left button of your mouse**. Drag the train to move the camera around.
 - Lastly the fourth camera, its the opposite of the first one, its a rearguard camera. 

In the second and third cameras you can zoom using the **scroll button** on your mouse.

Movement will also use **w,s,a,d** however its different form the person's movement.
W and S will not make you move at a fixed velocity, it will progressively increase until it reaches a cap. A and D require some velocity to turn the train, if there is no velocity the train will not turn. The train's movement is independent from the cameras.

You can press the **v** to go between rearguard camera and front view.

With **p** you can change the projection matrix of some cameras.
(Left overs from the previous code base, assignment 3).

## Mess with the shaders

You can change the trains texture! For that you will use the following keys:
(Please note that the trains texture is created at the beginning of the program, you will only mess with same parameters):

 - **1** key will increase the texture's noise scale.
 - **2** key will decrease texture's noise scale.
 - **3** key will increase the texture's ring frequency scale.
 - **4** key will decrease the texture's ring frequency scale.
 - **5** key will increase the texture's  ring sharpness scale.
 - **6** key will decrease the texture's ring sharpness scale.
 - **7** key will increase the texture's ring scale scale.
 - **8** key will decrease the texture's ring scale scale.

Note that the trains color is influenced by the light, if you are not seeing any changes try to see the other side of the train.

## Call a titan to go after you

Who doesn't love that? Press **t** to make a titan go after you, he will follow the person's position and he is faster than you. Pro tip: get on the train and run.

Sadly if he catches you, nothing will happen as I didn't implement yet.
To make him stop just press **t** again.

## Screenshot

You can screenshot the current camera view by pressing **ctr** + **p**.
The image will be at, looking from the main folder perspective:  <Screenshots/Screenshot.png>.

## Interesting point to note?

There are no collisions, so you can go against a mountain and pass through it.
The map is large but it ends. You can move through it, and you will not fall of the world.... yet. 