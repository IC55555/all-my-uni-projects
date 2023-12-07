from re import split
import matplotlib.patches
import matplotlib.path
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import csv
from math import *

from numpy import ravel_multi_index

X = []
Y = []

# with open('grid.txt', 'r') as datafile:
#     plotting = txt.reader(datafile, delimiter=';')
    
#     for ROWS in plotting:
#         X.append(float(ROWS[0]))
#         Y.append(float(ROWS[1]))

def drawLine(axes, x_center, y_center, x, y):

    x0 = x_center
    y0 = y_center

    x1 = x
    y1 = y


    line = plt.Line2D([x0, x1], [y0, y1], color="k")
    axes.add_line(line)

    #plt.text(0.5, 1.1, "Line2D", horizontalalignment="center")


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
    

a = []
x_point = []
y_point = []
f=open("grid.txt", "r").readlines()
for s in f:
    s=s.strip().split(" ")
    if len(s) > 1:    
      a.append(list(map(float, s)))
     
x_center = a[0][0]
y_center = a[0][1]
for i in range(1, 5):
    x_point.append(a[i][0])
    y_point.append(a[i][1])
n_line = a[5][0]   
k_line = a[5][1]
n_circle = a[6][0]
k_circle = a[6][1] 



#X.append(max(x_point))
#Y.append(min(y_point))
#r =  sqrt((X[0] - x_center)**2 + (Y[0] - y_center)**2)
temp = []
for i in range(len(x_point)):
    if y_point[i] == min(y_point):
        temp.append(x_point[i])
        
r_min = min(temp) - x_center
r_max = max(temp) - x_center

X_0 = []
Y_0 = []

X_1 = []
Y_1 = []

X_0.append(x_center + r_min)
Y_0.append(y_center)

X_1.append(x_center + r_max)
Y_1.append(y_center)

for i in range(1, int(n_line) + 1):
    X_0.append(x_center + r_min * cos(pi/2 * i / 3))
    Y_0.append(y_center + r_min * sin(pi/2 * i / 3))
    X_1.append(x_center + r_max * cos(pi/2 * i / 3))
    Y_1.append(y_center + r_max * sin(pi/2 * i / 3))
     


plt.xlim(-1, 13)
plt.ylim(-1, 13)
plt.grid()
    # ������� ������� ���
axes = plt.gca()
axes.set_aspect("equal")
#drawLine(axes)

plt.plot(1,1,'ro')

for i in range(4):
    drawArc(axes, x_center, y_center, x_point[i], y_point[i])
    

for i in range(1, int(n_circle)):
    drawArc(axes, x_center, y_center, min(x_point), y_center + r_min + (r_max - r_min) / n_circle * i) 
    

for i in range(len(X_0)):
    drawLine(axes, X_0[i], Y_0[i], X_1[i], Y_1[i])
    

# plt.show()
# plt.plot(X, Y)
# plt.title('Line Graph using CSV')
# plt.xlabel('X')
# plt.ylabel('Y')
plt.show()

