clear
close all
clc


%% Reading Data

data = readmatrix("04_19_2022 Data/TESTRUN4.TXT"); %Opening file for reading ('r')

time = data(:,1);
pos_in = data(:,2);
pos_out = data(:,3);
vel = data(:,4);
yaw = data(:,5);
current = data(:,6);

data = readmatrix("04_19_2022 Data/TESTRUN5.TXT"); %Opening file for reading ('r')

time2 = data(:,1);
pos_in2 = data(:,2);
pos_out2 = data(:,3);
vel2 = data(:,4);
yaw2 = data(:,5);
current2 = data(:,6);


figure();
subplot(2,1,1);
hold on;
plot(time, pos_in.*180/4096,'linewidth',2);
plot(time, pos_out,'linewidth',2);
hold off;
xlabel('Time [s]');
ylabel('Leg Angle [deg]');
title("Sinusoid with Break");
legend("Input","Output");

subplot(2,1,2);
hold on;
plot(time2, pos_in2.*180/4096,'linewidth',2);
plot(time2, pos_out2,'linewidth',2);
hold off;
xlabel('Time [s]');
ylabel('Leg Angle [deg]');
title("Pure Sinusoid");
legend("Input","Output");

figure();
subplot(2,2,1);
hold on;
plot(time, pos_in,'linewidth',2);
plot(time, pos_out,'linewidth',2);
hold off;
xlabel('Time [s]');
ylabel('Leg Angle [deg]');
title("Leg Angle Vs Time");
legend("Input","Output");

subplot(2,2,2);
plot(time, vel,'linewidth',2);
xlabel('Time [s]');
ylabel('Velocity [deg/s]');
title("Leg Angular Velocity Vs Time");

subplot(2,2,3);
plot(time, yaw,'linewidth',2);
xlabel('Time [s]');
ylabel('Yaw [deg]');
title("Yaw Vs Time");

subplot(2,2,4);
plot(time, current,'linewidth',2);
xlabel('Time [s]');
ylabel('Current [mA]');
title("Current Vs Time");





