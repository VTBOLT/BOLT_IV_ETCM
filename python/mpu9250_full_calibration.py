######################################################
# Copyright (c) 2021 Maker Portal LLC
# Author: Joshua Hrisko
######################################################
#
# This code reads data from the MPU9250/MPU9265 board
# (MPU6050 - accel/gyro, AK8963 - mag)
# and solves for calibration coefficients for the
# accelerometer, gyroscope, and magnetometer using
# various methods with the IMU attached to a 3D-cube
# (MPU9250 + calibration block)
# --- The calibration coefficients are then saved to
# --- a local .csv file, which can be loaded upon
# --- the next run of the program
#
#
######################################################
#
# wait 5-sec for IMU to connect
import time
t0 = time.time()
start_bool = False # if IMU start fails - stop calibration
while time.time()-t0<5:
    try: 
        from mpu9250_i2c import *
        start_bool = True
        break
    except:
        continue
import numpy as np
import csv,datetime
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

time.sleep(2) # wait for mpu to load
# 
#####################################
# Gyro calibration (Steady)
#####################################
#
def gyro_cal():
    print("-"*50)
    print('Gyro Calibrating - Keep the IMU Steady')
    [mpu6050_conv() for ii in range(0,cal_size)] # clear buffer between readings
    mpu_array = [] # imu array for gyro vals
    gyro_offsets = [0.0,0.0,0.0] # gyro offset vector
    while True:
        try:
            _,_,_,wx,wy,wz = mpu6050_conv() # read and convert mpu6050 data
        except:
            continue

        mpu_array.append([wx,wy,wz]) # gyro vector append

        if np.shape(mpu_array)[0]==cal_size:
            for qq in range(0,3):
                gyro_offsets[qq] = np.mean(np.array(mpu_array)[:,qq]) # calc gyro offsets
            break
    print('Gyro Calibration Complete')
    return gyro_offsets # return gyro coeff offsets
# 
#####################################
# Accel Calibration (gravity)
#####################################
#
def accel_fit(x_input,m_x,b):
    return (m_x*x_input)+b # fit equation for accel calibration
#
def get_accel():
    ax,ay,az,_,_,_ = mpu6050_conv() # read and convert accel data
    return ax,ay,az
    
def accel_cal():
    print("-"*50)
    print("Accelerometer Calibration")
    mpu_offsets = [[],[],[]] # offset array to be printed
    axis_vec = ['z','y','x'] # axis labels
    cal_directions = ["upward","downward","perpendicular to gravity"] # direction for IMU cal
    cal_indices = [2,1,0] # axis indices
    for qq,ax_qq in enumerate(axis_vec):
        ax_offsets = [[],[],[]]
        print("-"*50)
        for direc_ii,direc in enumerate(cal_directions):
            input("-"*8+" Press Enter and Keep IMU Steady to Calibrate the Accelerometer with the -"+\
              ax_qq+"-axis pointed "+direc)
            [mpu6050_conv() for ii in range(0,cal_size)] # clear buffer between readings
            mpu_array = []
            while len(mpu_array)<cal_size:
                try:
                    ax,ay,az = get_accel() # get accel variables
                    mpu_array.append([ax,ay,az]) # append to array
                except:
                    continue
            ax_offsets[direc_ii] = np.array(mpu_array)[:,cal_indices[qq]] # offsets for direction

        # Use three calibrations (+1g, -1g, 0g) for linear fit
        popts,_ = curve_fit(accel_fit,np.append(np.append(ax_offsets[0],
                                 ax_offsets[1]),ax_offsets[2]),
                   np.append(np.append(1.0*np.ones(np.shape(ax_offsets[0])),
                    -1.0*np.ones(np.shape(ax_offsets[1]))),
                        0.0*np.ones(np.shape(ax_offsets[2]))),
                            maxfev=10000)
        mpu_offsets[cal_indices[qq]] = popts # place slope and intercept in offset array
    print('Accelerometer Calibrations Complete')
    return mpu_offsets

if __name__ == '__main__':
    if not start_bool:
        print("IMU not Started - Check Wiring and I2C")
    else:
        #
        ###################################
        # input parameters
        ###################################
        #
        mpu_labels = ['a_x','a_y','a_z','w_x','w_y','w_z','m_x','m_y','m_z']
        cal_labels = [['a_x','m','b'],['a_y','m','b'],['a_z','m','b'],'w_x','w_y','w_z',
                      ['m_x','m_x0'],['m_y','m_y0'],['m_z','m_z0']]
        cal_filename = 'mpu9250_cal_params.csv' # filename for saving calib coeffs
        cal_size = 200 # how many points to use for calibration averages
        cal_offsets = np.array([[],[],[],0.0,0.0,0.0,[],[],[]]) # cal vector
        #
        ###################################
        # call to calibration functions
        ###################################
        #
        re_cal_bool = input("Input 1 + Press Enter to Calibrate New Coefficients or \n"+\
                            "Press Enter to Load Local Calibration Coefficients ")
        if re_cal_bool=="1":
            print("-"*50)
            input("Press Enter to Start The Calibration Procedure")
            time.sleep(1)
            gyro_offsets = gyro_cal() # calibrate gyro offsets under stable conditions
            cal_offsets[3:6] = gyro_offsets 
            mpu_offsets = accel_cal() # calibrate accel offsets 
            cal_offsets[0:3] = mpu_offsets
            # save calibration coefficients to file
            with open(cal_filename,'w',newline='') as csvfile:
                writer = csv.writer(csvfile,delimiter=',')
                for param_ii in range(0,len(cal_offsets)):
                    if param_ii>2:
                        writer.writerow([cal_labels[param_ii],cal_offsets[param_ii]])
                    else:
                        writer.writerow(cal_labels[param_ii]+
                                     [ii for ii in cal_offsets[param_ii]])
        else:
            # read calibration coefficients from file
            with open(cal_filename,'r',newline='') as csvfile:
                reader = csv.reader(csvfile,delimiter=',')
                iter_ii = 0
                for row in reader:
                    if len(row)>2:
                        row_vals = [float(ii) for ii in row[int((len(row)/2)+1):]]
                        cal_offsets[iter_ii] = row_vals
                    else:
                        cal_offsets[iter_ii] = float(row[1])
                    iter_ii+=1
        #
        ###################################
        # print out offsets for each sensor
        ###################################
        #
        print("-"*50)
        print('Offsets:')
        for param_ii,param in enumerate(cal_labels):
            print('\t{0}: {1}'.format(param,cal_offsets[param_ii]))
        print("-"*50)
