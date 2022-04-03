close all
clc
clear all

t_animation = 0:0.05:10;

%Updating parameters for sinusoid
A_left = 140;
A_right = 140;
phi = 0;
offset = 140;

f_left = 1.1;
f_right = 1.5;

w_left = 2*pi*f_left;
w_right = 2*pi*f_right;

p_right = 1/f_right;
p_left = 1/f_left;

period_total = 1/(2*f_left)+1/(2*f_right); %Total period
t = linspace(0,period_total-0.00001,10000);

pos_right = zeros(1,10000);
pos_left = zeros(1,10000);


%Calculating positions for each period
for i = 1:length(t(1,:))
    tau = mod(t(1,i),period_total);

    %First half
    if (tau < p_right/2)
        pos_right(i) = A_right.*sin(w_right.*t(i) + phi) + offset;

        %Second half
    elseif (tau >= p_right/2)&&(tau <= p_right/2+2*p_left/2)
        pos_left(i) = A_left.*sin(w_left.*(t(i)-p_right/2+p_left/2) + phi) + offset;
        pos_right(i) = pos_left(i);

    end
end


%Plotting Data
t_left_curr = t(pos_left(1,:) > 0);
pos_left_curr = pos_left(1,:);
pos_left_curr = pos_left_curr(pos_left_curr > 0);

figure();
hold on;
plot(t(1,:),pos_right(1,:),'linewidth',2);
plot(t_left_curr,pos_left_curr,'linewidth',2);
plot((p_right/4).*ones(1,100),linspace(offset-A_right,offset+A_right,100),'--k');
plot((p_right/4+p_left/2).*ones(1,100),linspace(offset-A_left,offset+A_left,100),'--k');

plot(t(1,:),offset.*ones(1,length(t(1,:))),'--k'); %X-axis
hold off;


