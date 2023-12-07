
import numpy as np
import matplotlib.pyplot as plt

def spline_drawing():

    data = []
with open("data.txt") as f:
    for line in f:
        data.append([float(x) for x in line.split()])

    #for x in np.arange(x1, x2, 0.01):
            #s = cubic_lagrange(x, x0, x1, x2, x3, y0, y1, y2, y3)
            #s += lambda_val * ((x - x1) * (x - x1) * (mu * (x2 - x) + nu * (x - x1)))
            #y_spline.append(s)

            #return y_spline

# Определение данных точек
x_data = [0.0, 1.0, 2.0, 3.0, 4.0]
y_data = [0.0, 1.0, 4.0, 9.0, 16.0]

# Задание параметра сглаживания lambda
lambda_val = 0.5

# Получение базиса сплайна
y_spline = smoothing_spline(x_data, y_data, lambda_val)

# Построение графика функции по базису сплайна
plt.plot(y_spline)
plt.show()