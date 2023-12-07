from encodings import utf_8
coding: utf_8 
# Commented out IPython magic to ensure Python compatibility.
from statistics import mean
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from scipy.stats import mode
from scipy.stats import kurtosis
from scipy.stats import skew
import scipy.stats as st
from scipy.optimize import minimize
import seaborn as sns
import os
import pandas_profiling
import statistics
import sympy
from scipy import integrate
import math
import random
# %matplotlib inline

k_from_nu = { 0.001: 2.63}


def f(x, nu):
  k = k_from_nu.get(nu)
  if(k==None):
     print("don't know k")
     return -1
  if(abs(x) <= k):
     return (1.0-nu)/(math.sqrt(2.0*math.pi))*math.exp(-x*x/2.0)
  else:
    return (1.0-nu)/(math.sqrt(2.0*math.pi))*math.exp(1.0/2.0*k*k - k*abs(x))

def dfdf_f(x,nu):
  k = k_from_nu.get(nu)
  if(k==None):
    print(" don't know k ")
    return -1
  if(abs(x) <= k):
    return (1.0-nu)/(math.sqrt(2.0*math.pi))*math.exp(-x*x/2.0)*(x*x)
  else:
    return (1.0-nu)/(math.sqrt(2.0*math.pi))*math.exp(1.0/2.0*k*k - k*abs(x))*(k*k)  
  
def influence_OMP(x, nu):
  k = k_from_nu.get(nu)
  if(k==None):
    print(" don't know k ")
    return -1
  v, err = integrate.quad(dfdf_f, -np.inf, np.inf, args = (nu))
  if(abs(x) <= k):
    return x / v
  else:
    return k * math.copysign(1, x)  

def dfdff(x,nu, delta):
  k = k_from_nu.get(nu)
  if(k==None):
    print(" don't know k ")
    return -1
  if(abs(x) <= k):
    return (1.0-nu)/(math.sqrt(2.0*math.pi))*math.exp(-(delta+1)*x*x/2.0)*(x*x)
  else:
    return (1.0-nu)/(math.sqrt(2.0*math.pi))*math.exp(((delta+1))*(1.0/2.0*k*k -k*abs(x)))*(k*k)  

def influence_rad(x, nu, delta):
  k = k_from_nu.get(nu)
  if(k==None):
    print(" don't know k ")
    return -1
  v, err = integrate.quad(dfdf_f, -np.inf, np.inf, args = (nu))
  if(abs(x) <= k):
    return (math.exp(-delta*x*x/2.0)*x) / v
  else:
    return (math.exp(delta*(1.0/2.0*k*k - k*abs(x)))*k * math.copysign(1, x)) / v

def f_noise(x, nu, eps, m_noise, D_noise):
  return (1.0-eps)*f(x , nu) + eps*f((x - m_noise)/D_noise, nu)/ D_noise

def Q(theta, data_ksi, nu, delta, eps,m_noise,D_noise):
 result_Q = 0
 N = len(data_ksi)
 if(eps == 0.0):
  for i in range(0, N-1):
   result_Q += -(1.0 / f(0,nu)**delta)*(f((data_ksi[i]-theta),nu)**delta)
 else:
  for i in range(0, N-1):
   result_Q += -(1.0 / f_noise(0,nu,eps,m_noise,D_noise)**delta)*(f_noise((data_ksi[i]- theta),nu,eps,m_noise,D_noise)**delta)
 return result_Q

def genericvalue(nu, k, P):
  r = random.uniform(0, 1)
  if(r>= P):
   x = random.normalvariate(0,1)
   while(-k > x or x > k):
    x = random.normalvariate(0,1)
   return x
  else:
   r2 = random.uniform(0, 1)
   x = k - math.log(r2)/k
   if(r < P/2):
    return x
   else:
    return -x

N = 1000000 # объем выборки
#N = 1000 # объем выборки

nu = 0.001 # требуемый параметр формы
k = k_from_nu.get(nu)
if(k==None):
 print(" don't know k ")
 exit(1)
P = 2*(1-nu)/k*f(k, nu)

data = [] # генерация "чистых данных" (массив упорядочен повозрастанию)
for i in range(1, N):
 data.append(genericvalue(nu,k,P))
data.sort()
data_f = [] # значения функции плотности для данных data
for i in range(0, N-1):
 data_f.append(f(data[i], nu))
 
print("0.05 ind", math.floor(N*0.05), "k", data[-math.floor(N*0.05)])
print("0.10 ind", math.floor(N*0.10), "k", data[-math.floor(N*0.10)])
print("0.15 ind", math.floor(N*0.15), "k", data[-math.floor(N*0.15)])

m = mean(data)
print("Среднее", m)
print("Медиана", statistics.median(data))
print("Дисперсия", statistics.variance(data, m))
print("Асимметрия:", skew(data))
print("Эксцесс:", kurtosis(data, fisher= False))
print("Макс:",max(data))
print("Min:",min(data))

N = 1000 # объем выборки
nu_noise = 0.001 # требуемый параметр формы
k_noise = k_from_nu.get(nu_noise) # соответствующий параметру формы
P_noise = 2*(1-nu_noise)/k_noise*f(k_noise, nu_noise)
m_noise = 3.0 # theta
D_noise = 1.5 # lambda
noise = [] # генерация "шума" (массив упорядочен по возрастанию)
for i in range(1, N):
 noise.append(m_noise+D_noise*genericvalue(nu_noise,k_noise,P_noise))
noise.sort()

noise_f = [] # значения функции плотности для данных data
for i in range(0, N-1):
 noise_f.append(f((noise[i]-m_noise) /D_noise, nu_noise) / D_noise)

N = 1000 # объем выборки
# зашумленные данные
data_with_noise = []
nu = 0.001 # требуемый параметр формы
k = k_from_nu.get(nu) # соответствующий параметру формы
P = 2*(1-nu)/k*f(k, nu)
nu_noise = 0.001 # требуемый параметр формы
k_noise = k_from_nu.get(nu_noise) # соответствующий параметру формы
P_noise = 2*(1-nu_noise)/k_noise*f(k_noise, nu_noise)
#m_noise = 3.0
#D_noise = 1.5
m_noise = 3.0 # theta
D_noise = 1.5 # lambda
eps = 0.1
noise_count = 0;
for i in range(1, N):
 r = random.uniform(0.0, 1.0)
 if(r <= 1 - eps):
  data_with_noise.append(genericvalue(nu,k,P))
 else:
  data_with_noise.append(m_noise + D_noise*genericvalue(nu_noise,k_noise,P_noise))
  noise_count+=1
  
data_with_noise.sort()
print(noise_count)
data_with_noise_f = [] # значения функции плотности для данных data

for i in range(0, N-1):
 data_with_noise_f.append(f_noise(data_with_noise[i], nu, eps, m_noise,D_noise))
 
 """Отображение графиков (зеленый- просто данные, красный - просто шум, синий
- зашумленные данные)"""
plt.plot(data, data_f, 'g')
plt.plot(noise, noise_f,'r')
plt.plot(data_with_noise,data_with_noise_f,'b')
plt.legend({"data", "noise", "data with noise"})
plt.show()

"""Статистические характеристики и оценки (менять массив testing_data и матожидание-дисперсию-уровень зашумления рассматриваемой выборки, можно менять
proportiontocut, delta)"""
#testing_data = data_with_noise
testing_data = data_with_noise
m_noise = 3
D_noise = 1.5
#eps = 0.3
eps = 0.1

m = mean(testing_data)
print("Среднее", m)
print("Медиана", statistics.median(testing_data))
print("Дисперсия", statistics.variance(testing_data, m))
print("Асимметрия:", skew(testing_data))
print("Эксцесс:", kurtosis(testing_data, fisher= False))
print("Макс:",max(testing_data))
print("Min:",min(testing_data))
print("Оценка максимального правдоподобия:", st.norm.fit(testing_data)[0]) #первое - матожидание, второе - дисперсия

proportiontocut = 0.05

print("Усеченное среднее:", st.trim_mean(testing_data, proportiontocut), "уровень усечения", proportiontocut)

proportiontocut = 0.1

print("Усеченное среднее:", st.trim_mean(testing_data, proportiontocut), "уровень усечения", proportiontocut)

proportiontocut = 0.15

print("Усеченное среднее:", st.trim_mean(testing_data, proportiontocut), "уровень усечения", proportiontocut)

delta = 0.1
m_start = m
res = minimize(Q, m_start, args = (testing_data, nu, delta, eps, m_noise, D_noise), method='nelder-mead')#???
print("Обобщенная радикальная оценка:", res.x[0], "; параметр delta", delta, " Q = ", Q(res.x[0],testing_data, nu, delta, eps, m_noise, D_noise))

delta = 0.5
m_start = m

res = minimize(Q, m_start, args = (testing_data, nu, delta, eps, m_noise, D_noise), method='nelder-mead')#???
print("Обобщенная радикальная оценка:", res.x[0], "; параметр delta", delta, " Q = ", Q(res.x[0],testing_data, nu, delta, eps, m_noise, D_noise))

delta = 1
m_start = m

res = minimize(Q, m_start, args = (testing_data, nu, delta, eps, m_noise, D_noise), method='nelder-mead')#???

print("Обобщенная радикальная оценка:", res.x[0], "; параметр delta", delta, " Q = ", Q(res.x[0],testing_data, nu, delta, eps, m_noise, D_noise))

avg_infl = []
mediana_infl = []
avg005_infl = []
avg010_infl = []
avg015_infl = []
OMP_infl = []
rad01_infl = []
rad05_infl = []
rad10_infl = []
h_d = []
N2 = 10000
start = -8 #min(testing_data)-1.0
#h = (max(testing_data) + 1.0 - (min(testing_data)-1.0)) / N2
h = (13 - (-8)) / N2

for i in range (0, N2):
 y = start + i*h
 h_d.append(y)
 avg_infl.append(y)
 mediana_infl.append(math.copysign(1, y) / (2.0*f(0,nu)))
 tmp = 1.0/(1.0-2.0*0.05) # a = 0.05; k = 1.65
 if(y <= -1.65):
  tmp *= -1.65
 else:
   if(y >= 1.65):
    tmp *= 1.65
   else:
    tmp *= y
    
 avg005_infl.append(tmp)
 tmp = 1.0/(1.0-2.0*0.1) # a = 0.1; k = 1.28
 
 if(y <= -1.28):
  tmp *= -1.28
 else:
  if(y >= 1.28):
   tmp *= 1.28
  else:
   tmp *= y
   
 avg010_infl.append(tmp)
 tmp = 1.0/(1.0-2.0*0.15) # a = 0.15; k = 1.04
 
 if(y <= -1.04):
  tmp *= -1.04
 else:
  if(y >= 1.04):
   tmp *= 1.04
  else:
   tmp *= y
 avg015_infl.append(tmp)
 OMP_infl.append(influence_OMP(y, nu))
 delta = 0.1
 rad01_infl.append(influence_rad(y, nu, delta))
 delta = 0.5
 rad05_infl.append(influence_rad(y, nu, delta))
 delta = 1
 rad10_infl.append(influence_rad(y, nu, delta))
 
 plt.plot(h_d, avg_infl, 'g', label = "avg_0")
plt.plot(h_d, mediana_infl,'r', label = "mediana")
plt.plot(h_d,avg005_infl, label = "avg_0.05")
plt.plot(h_d,avg010_infl, label = "avg_0.10")
plt.plot(h_d,avg015_infl, label = "avg_0.15")
plt.plot(h_d,OMP_infl, label = "OMP")
plt.plot(h_d,rad01_infl, label = "rad_0.1")
plt.plot(h_d,rad05_infl, label = "rad_0.5")
plt.plot(h_d,rad10_infl, label = "rad_1.0")

plt.legend()
plt.show()

plt.plot(h_d, avg_infl, label = "avg_0")

plt.legend()
plt.show()

plt.plot(h_d, mediana_infl,label = "mediana")

plt.legend()
plt.show()

plt.plot(h_d, avg_infl, label = "avg_0")
plt.plot(h_d,OMP_infl, label = "OMP")
plt.xticks(np.arange(-8,13,1))

plt.legend()
plt.show()

plt.plot(h_d,avg005_infl, label = "avg_0.05")
plt.plot(h_d,avg010_infl, label = "avg_0.10")
plt.plot(h_d,avg015_infl, label = "avg_0.15")

plt.legend()
plt.show()

plt.plot(h_d,rad01_infl, label = "rad_0.1")
plt.plot(h_d,rad05_infl, label = "rad_0.5")
plt.plot(h_d,rad10_infl, label = "rad_1.0")

plt.legend()
plt.show()