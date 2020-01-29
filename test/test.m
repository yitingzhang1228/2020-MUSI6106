% Yiting Zhang - HW1
clear all;

%% Real-World example

% audio file 1
[sweep,Fs] = audioread('sweep.wav');
[sweep_FIR,Fs] = audioread('sweep_FIR.wav'); % delay = 0.5s, gain = 0.5
[sweep_IIR,Fs] = audioread('sweep_IIR.wav'); % delay = 0.5s, gain = 0.5

% audio file 2
[brahms,Fs] = audioread('Brahms.wav');
[brahms_FIR,Fs] = audioread('Brahms_FIR.wav'); % delay = 1s, gain = 0.6
[brahms_IIR,Fs] = audioread('Brahms_IIR.wav'); % delay = 1s, gain = 0.6

% FIR Comb Filter sweep
x=sweep(:,1); 
g=0.5;
d=0.5*Fs;
Delayline=zeros(d,1);

for n=1:length(x)
    y(n)=x(n)+g*Delayline(d);
    Delayline=[x(n);Delayline(1:d-1)];
end

sweep_FIR_diff = abs(y' - sweep_FIR(:,1));
err_sweep_FIR = immse(y',sweep_FIR(:,1));

figure;
plot(sweep_FIR_diff);
title('FIR difference for sweep.wav');


% IIR Comb Filter sweep
x=sweep(:,1); 
g=0.5;
d=0.5*Fs;
Delayline=zeros(d,1);

for n=1:length(x)
    y(n)=x(n)+g*Delayline(d);
    Delayline=[y(n);Delayline(1:d-1)];
end

sweep_IIR_diff = abs(y' - sweep_IIR(:,1));
err_sweep_IIR = immse(y',sweep_IIR(:,1));

figure;
plot(sweep_FIR_diff);
title('IIR difference for sweep.wav');



% FIR Comb Filter brahms
x=brahms(:,1);
g=0.6;
d=1*Fs;
Delayline=zeros(d,1);

for n=1:length(x)
    y(n)=x(n)+g*Delayline(d);
    Delayline=[x(n);Delayline(1:d-1)];
end

brahms_FIR_diff = abs(y' - brahms_FIR(:,1));
err_brahms_FIR = immse(y',brahms_FIR(:,1));

figure;
plot(brahms_FIR_diff);
title('FIR difference for brahms.wav');


% IIR Comb Filter brahms
x=brahms(:,1); 
g=0.6;
d=1*Fs;
Delayline=zeros(d,1);

for n=1:length(x)
    y(n)=x(n)+g*Delayline(d);
    Delayline=[y(n);Delayline(1:d-1)];
end

brahms_IIR_diff = abs(y' - brahms_IIR(:,1));
err_brahms_IIR = immse(y',brahms_IIR(:,1));

figure;
plot(brahms_IIR_diff);
title('IIR difference for brahms.wav');

%% Generate sine wave for testing
f=440;
Amp=0.5;
ts=1/44100;
Fs=44100;
T=3;
t=0:ts:T;
y=Amp*sin(2*pi*f*t);
audiowrite('/Users/Christine/Desktop/sine_440.wav',y,Fs);

%% Generate zero input signal for testing
y = zeros(Fs*3,1);
audiowrite('/Users/Christine/Desktop/silence.wav',y,Fs);