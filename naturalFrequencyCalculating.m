close all
clear all
clc

%%

% Read data
data = csvread('Take 2022-04-12 04.15.25 PM.csv', 8);

t = data(:,2); % Time in seconds
orientation_raw = data(:,[5,4,3,11,10,9]); % Left leg and right leg roll, pitch, yaw
orientation_raw;

orientation_filtered = orientation_raw;

% Correct the Euler Angle order if they're incorrect (skip this if they're
% correct)
%{
for ii = 1:size(orientation_raw,1)
    % Flip the leg yaw if mocap tracked it backwards
    if orientation_raw(ii,3) < - 70
        orientation_raw(ii,[1,3]) = -orientation_raw(ii,[1,3]);
    end
    if orientation_raw(ii,6) < - 70
        orientation_raw(ii,[4,6]) = -orientation_raw(ii,[4,6]);
    end
        
    Rl = eul2rotm(orientation_raw(ii,1:3)*pi/180,'XYZ');
    Rr = eul2rotm(orientation_raw(ii,4:6)*pi/180,'XYZ');
    
    orientation_filtered(ii,[3,2,1]) = rotm2eul(Rl,'ZYX')*180/pi;
    orientation_filtered(ii,[6,5,4]) = rotm2eul(Rr,'ZYX')*180/pi;
end
%}

% Apply a median filter to try to get rid of spikes
orientation_filtered = medfilt2(orientation_filtered,[3,1]);



figure(1)
clf
% subplot(3,1,1)
plot(t,orientation_filtered(:,[1,4]));
ylabel('Roll (deg)')
axis([7 30 -24 24]);
% subplot(3,1,2)
% plot(t,orientation_filtered(:,[2,5]));
% ylabel('Pitch (deg)')
% axis([7 40 -10 10]);
% subplot(3,1,3)
% plot(t,orientation_filtered(:,[3,6]));
% ylabel('Yaw (deg)')
% xlabel('Time (s)')
% axis([7 40 -65 10]);

sgtitle('Same Sphere, 4 Hz, 128');


%%

t = [21.6833 22.1889 22.8];
theta = [13.1598 1.58525 0.529654];

Tp = t(2:end) - t(1:end-1);
wn = 1./Tp




