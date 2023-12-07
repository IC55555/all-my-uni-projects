from re import split
import matplotlib.patches
import matplotlib.path
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import csv
from math import *
from numpy import ravel_multi_index


def input(X_points, Y_points):
    f=open("out.txt", "r").readlines()
    for chars in f:
        chars=chars.strip().split()
        X_points.append(chars[0])
        Y_points.append(chars[1])
        

def drawLine(axes, x_start, y_start, x, y):

    x0 = x_start
    y0 = y_start

    x1 = x 
    y1 = y

    line = plt.Line2D([x0, x1], [y0, y1], color="k")
    axes.add_line(line)

def radius_find(x_center, y_center, X_points, Y_points, X_0, Y_0, X_1, Y_1):
    temp = []
    for i in range(len(X_points)):
        if Y_points[i] == min(Y_points):
            temp.append(X_points[i])
        
    r_min = min(temp) - x_center
    r_max = max(temp) - x_center

    X_0.append(x_center + r_min)
    Y_0.append(y_center)

    X_1.append(x_center + r_max)
    Y_1.append(y_center)

    for i in range(1, 4):
        X_0.append(x_center + r_min * cos(pi/2 * i / 3))
        Y_0.append(y_center + r_min * sin(pi/2 * i / 3))
        X_1.append(x_center + r_max * cos(pi/2 * i / 3))
        Y_1.append(y_center + r_max * sin(pi/2 * i / 3))

def drawArc(axes, x_center, y_center, x, y):
    
    d = 2 * sqrt((x - x_center)**2 + (y - y_center)**2)
    
    arc_x = x_center
    arc_y = y_center
    
    arc_width = d
    arc_height = d
    arc_theta1 = 0
    arc_theta2 = 90

    arc = matplotlib.patches.Arc((arc_x, arc_y),
                                 arc_width,
                                 arc_height,
                                 theta1=arc_theta1,
                                 theta2=arc_theta2)
    axes.add_patch(arc)
    #plt.text(0.6, -0.3, "Arc", horizontalalignment="center")  

def normal(axes, X_0, Y_0, X_1, Y_1, X_points, Y_points, x_center, y_center):
    for i in range(len(X_0)):
        drawLine(axes, X_0[i], Y_0[i], X_1[i], Y_1[i])
    
    for i in range(len(X_points)):
        drawArc(axes, x_center, y_center, X_points[i], Y_points[i])
    

def discret(x_center, y_center, X_points, Y_points, X_global, Y_global):
    left_border_X = []
    left_border_Y = []
    for i in range(len(X_points)):
        if X_points[i] == min(X_points):
            left_border_X.append(X_points[i])
            left_border_Y.append(Y_points[i])
            
    for i in range(len(left_border_X)):
        X = [left_border_X[i]]
        Y = [left_border_Y[i]]
        for j in range(len(X_points)):
            L1 = left_border_Y[i] - y_center
            L2 = sqrt((X_points[j] - x_center)**2 + (Y_points[j] - y_center)**2)
            if L1 == L2:
                X.append(X_points[j])
                Y.append(Y_points[j])
        X_global.append(X) 
        Y_global.append(Y)  
        print(X, Y)
    
        
    

def output(X_points, Y_points):
    
    plt.xlim(0, 13)
    plt.ylim(0, 13)
    plt.grid()
    
    plt.xlabel('X')
    plt.ylabel('Y')
    for i in range(len(X_points)):
        plt.plot(X_points[i], Y_points[i], "ro")
        
    plt.show()
    



def main():
    X_points = []
    Y_points = []
    
    X_0 = []
    Y_0 = []

    X_global = []
    Y_global = []

    X_1 = []
    Y_1 = []    

    axes = plt.gca()
    axes.set_aspect("equal")
    
    input(X_points, Y_points)
    X_points = list(map(float, X_points))  
    Y_points = list(map(float, Y_points))
    x_center = min(X_points)
    y_center = min(Y_points)
    radius_find(x_center, y_center, X_points, Y_points, X_0, Y_0, X_1, Y_1)
    
    #normal(axes, X_0, Y_0, X_1, Y_1, X_points, Y_points, x_center, y_center) 
    
    discret(x_center, y_center, X_points, Y_points, X_global, Y_global)

    for i in range(len(X_global)):
        for j in range(len(X_global[i]) - 1):
            drawLine(axes, X_global[i][j], Y_global[i][j], X_global[i][j+1], Y_global[i][j+1])

    output(X_points, Y_points)
    
main()
 