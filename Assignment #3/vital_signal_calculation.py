from datetime import datetime
import time

import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, lfilter
from scipy import interpolate
import pandas as pd
import math

measures = {}
working_data = {}

#Get Date
def getData(filename, column_name = "None",delim=','):
        if(column_name != 'None'):
            hrdata = np.genfromtxt(filename, delimiter=delim, names=True, dtype=None)

            try:
                irdata = hrdata[column_name]
            except:
                print('\nError loading column "%s"' %column_name)
        elif (column_name == 'None'):
            hrdata = np.genfromtxt(filename, delimiter=delim, names=True, dtype=None)
        else:
            print('\nError: file not found')
        return hrdata


def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a


def butter_bandpass_filter(data, lowcut, highcut, fs, order=5):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

def rollwindow(x, window):
    shape = x.shape[:-1] + (x.shape[-1] - window + 1, window)
    strides = x.strides + (x.strides[-1],)
    return np.lib.stride_tricks.as_strided(x, shape=shape, strides=strides)

def rolmean(hrdata, hrw, fs):
    avg_hr = (np.mean(hrdata))
    print(avg_hr)
    hrarr = np.array(hrdata)
    rol_mean = np.mean(rollwindow(hrarr, int(hrw * fs)), axis=1)
    print((abs(len(hrarr) - len(rol_mean)) / 2))
    ln = np.array([int(avg_hr) for i in range(0, int(abs(len(hrarr) - len(rol_mean)) / 2))])
    rol_mean = np.insert(rol_mean, 0, ln)
    rol_mean = np.append(rol_mean, ln)
    # rol_mean = rol_mean * 0.9545
    rol_mean = rol_mean * 1.1
    return rol_mean



def detect_peaks(hrdata, rol_mean, ma_perc, fs):
    rm = np.array(rol_mean)
    rolmean = rm + ((rm / 100) * ma_perc)
    # print(hrdata,rolmean)
    # plt.plot(Time[start1:start2], rolmean[start1:start2], color='Green')
    peaksx = np.where((hrdata > rolmean))[0]
    # print('x',peaksx[100:200])
    # print('diff',np.diff(peaksx)[100:200])
    peaksy = hrdata[np.where((hrdata > rolmean))[0]]
    peakedges = np.concatenate((np.array([0]), (np.where(np.diff(peaksx) > 1)[0]), np.array([len(peaksx)])))
    # print(peakedges)
    peaklist = []
    ybeat = []
    for i in range(0, len(peakedges) - 1):
        try:
            y = peaksy[peakedges[i]:peakedges[i + 1]].tolist()
            peaklist.append(peaksx[peakedges[i] + y.index(max(y))])
        except:
            pass
    # working_data['peaklist'] = peaklist
    # working_data['ybeat'] = [hrdata[x] for x in peaklist]
    # # plt.plot(peaklist[10:50],ybeat[10:50])
    # working_data['rolmean'] = rolmean
    return peaklist
    # calculateRR(fs)
    # if len(working_data['RR_list']):
    #     working_data['rrsd'] = np.std(working_data['RR_list'])
    # else:
    #     working_data['rrsd'] = np.inf

def calculateRR(data, rol_mean, fs):
    ma_perc_list = [5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 150, 200, 300]
    hr = []
    for x in ma_perc_list:
        num = 0
        peaklist = detect_peaks(data, rol_mean, x, fs)
        # print(peaklist)
        for peak in peaklist:
            if(1500 < peak < 4500):
                num+=1
        hr.append(num)
    print(hr)
    print('Respiration Rate',np.mean(hr))
    return np.mean(hr)

def calculateHR(data, rol_mean, fs):
    ma_perc_list = [5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 150, 200, 300]
    hr = []
    for x in ma_perc_list:
        peaklist = detect_peaks(data, rol_mean, x, fs)
        # print('diff x',x, np.diff(peaklist))
        distance = np.mean(np.diff(peaklist))
        hrx = 60 * 50 / distance
        hr.append(hrx)

    return np.mean(hr)

    #     bpm = ((len(peaklist) / (len(filtered) / fs)) * 60)
    #     rrsd.append([peaklist, bpm, x])
    #
    # for x, y, z in rrsd:
    #     if ((x > 1) and ((y > 40) and (y < 150))):
    #         valid_ma.append([x, z])
    #
    # working_data['best'] = min(valid_ma, key=lambda t: t[0])[1]
    # return detect_peaks(hrdata, rol_mean, min(valid_ma, key=lambda t: t[0])[1], fs)



def minx(data, start):
    min = 0
    count = 0
    for y in data:
        if(y < min):
            min = y
            count += 1
        else: count +=1
    return start + count


def findRatio(IR, IRpeakindex, RED, REDpeakindex):

    IRmin = []
    AC_IRs =[]
    DC_IRs = []
    REDmin =[]
    AC_REDs = []
    DC_REDs = []
    minpointy = []
    minpointx = []
    for i in range(0, len(IRpeakindex)- 1):
        minpointx.append(minx(IR[IRpeakindex[i]:IRpeakindex[i + 1]], IRpeakindex[i]))
        minpointy.append(min(IR[IRpeakindex[i]:IRpeakindex[i + 1]]))
        listx = []
        listy = []
        if(i == 0 ):
            AC_IRs.append(IR[IRpeakindex[i]] - minpointy)
            DC_IRs.append(minpointy)
        else:
            listx.append(minpointx[i-1])
            listx.append(minpointx[i])
            listy.append(minpointy[i-1])
            listy.append(minpointy[i])
            f = interpolate.interp1d(listx,listy)
            DC_IRs.append(f(IRpeakindex[i]))
            AC_IRs.append(IR[IRpeakindex[i]] - f(IRpeakindex[i]))
        # IRmin.append(minpointy)
        # AC_IRs.append(IR[IRpeakindex[i]]-minpointy)
        # DC_IRs.append(minpointy)

    # for i in range(0, len(REDpeakindex) - 1):
    #     minpointy = min(RED[REDpeakindex[i]:REDpeakindex[i + 1]])
    #     REDmin.append(minpointy)
    #     AC_REDs.append(RED[REDpeakindex[i]] - minpointy)
    #     DC_REDs.append(minpointy)
    minpointx = []
    minpointy = []
    for i in range(0, len(REDpeakindex)- 2):
        minpointx.append(minx(RED[REDpeakindex[i]:REDpeakindex[i + 1]], REDpeakindex[i]))
        minpointy.append(min(RED[REDpeakindex[i]:REDpeakindex[i + 1]]))
        listx = []
        listy = []
        if(i == 0 ):
            AC_REDs.append(RED[REDpeakindex[i]] - minpointy)
            DC_REDs.append(minpointy)
        else:
            listx.append(minpointx[i-1])
            listx.append(minpointx[i])
            listy.append(minpointy[i-1])
            listy.append(minpointy[i])
            f = interpolate.interp1d(listx,listy)
            DC_REDs.append(f(REDpeakindex[i]))
            AC_REDs.append(RED[REDpeakindex[i]] - f(REDpeakindex[i]))

    AC_IR = np.array(AC_IRs)
    DC_IR = np.array(DC_IRs)
    AC_RED = np.array(AC_REDs)
    DC_RED = np.array(DC_REDs)
    R = (AC_RED.dot(DC_IR.T)/AC_IR.dot(DC_RED.T))
    print('R',R)
    return R
    # print(len(DC_IRs),len(IRmin))




def check_peaks():
    RR_arr = np.array(working_data['RR_list'])
    peaklist = np.array(working_data['peaklist'])
    peaklist2 = working_data['peaklist']
    ybeat = np.array(working_data['ybeat'])
    upper_threshold = np.mean(RR_arr) + 300
    lower_threshold = np.mean(RR_arr) - 300
    working_data['RR_list_cor'] = RR_arr[np.where((RR_arr > lower_threshold) & (RR_arr < upper_threshold))]
    peaklist_cor = peaklist[np.where((RR_arr > lower_threshold) & (RR_arr < upper_threshold))[0] + 1]
    working_data['peaklist_cor'] = np.insert(peaklist_cor, 0, peaklist[0])
    working_data['removed_beats'] = peaklist[np.where((RR_arr <= lower_threshold) | (RR_arr >= upper_threshold))[0] + 1]
    working_data['removed_beats_y'] = removed_beats_y = ybeat[
        np.where((RR_arr <= lower_threshold) | (RR_arr >= upper_threshold))[0] + 1]


data = getData('data.csv')
# time = data['time(second)']
hrw = 0.75
fs = 50
Time = data['TIME']
IR = data['IR']
RED = data['RED']
filteredHR = butter_bandpass_filter(IR, 1, 2, 50, order=5)
filteredHR_IR = filteredHR
filteredHR_RED = butter_bandpass_filter(RED, 1, 2, 50, order=5)
filteredRR = butter_bandpass_filter(RED, 1/6, 1/3, 50, order=4)
filteredRR_IR = filteredRR
filteredRR_RED= butter_bandpass_filter(RED, 1/6, 1/3, 50, order=4)
# plt.plot(Time[3000:3500],filteredHR[3000:3500])
# plt.plot(IR[3000:3500])
# plt.plot(IR)
# print(filtered)
start1 = 1500
start2 = 3000
plt.subplot(211)
plt.plot(Time[start1:start2],IR[start1:start2], color='Blue', alpha=0.5, label='Original Signal')
plt.legend(loc=4)
plt.subplot(212)
plt.plot(Time[start1:start2],filteredHR[start1:start2],color='Red', label='Filtered Signal')
plt.legend(loc=4)

# plt.plot(Time[start1:start2],filteredHR_IR[start1:start2], color='Blue', alpha=0.5, label='filtered from IR Signal')
# plt.legend(loc=4)
# plt.plot(Time[start1:start2],filteredHR_RED[start1:start2],color='Red', label='Red Signal')
# plt.legend(loc=4)

rol_meanHR_RED = rolmean(filteredHR_RED, hrw, fs)
rol_meanRR_RED = rolmean(filteredRR_RED, hrw, fs)
rol_meanHR_IR = rolmean(filteredHR_IR, hrw, fs)
heartrate = calculateHR(filteredHR, rol_meanHR_IR, fs)
respirationrate = calculateRR(filteredRR, rol_meanRR_RED, fs)
IRpeaklist = detect_peaks(filteredHR_IR, rol_meanHR_IR,60,fs)
REDpeaklist = detect_peaks(filteredHR_RED, rol_meanHR_RED,60,fs)
Ratio = findRatio(filteredHR_IR, IRpeaklist, filteredHR_RED, REDpeaklist)

# # calculate Ratio with RED
# rol_meanRR_RED = rolmean(filteredRR_RED, hrw, fs)
# rol_meanRR_IR = rolmean(filteredRR_IR, hrw, fs)
# IRpeaklist = detect_peaks(filteredRR_IR, rol_meanRR_IR,20,fs)
# REDpeaklist = detect_peaks(filteredRR_RED, rol_meanRR_RED,20,fs)
# Ratio = findRatio(filteredRR_IR, IRpeaklist, filteredRR_RED, REDpeaklist)

SPO2 = -45.060*Ratio* Ratio + 30.354 *Ratio + 94.845
print('SPO2',SPO2)
# print('diff', np.diff(peaklist))
# distance = np.mean(np.diff(peaklist))
# heartrate = 60 * 50 / distance
# print(heartrate)
plt.show()