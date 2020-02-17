close all;
clear all;

% set Delay=0.5 in vibrato.m

%% test1: Ah_mono.wav
[y,Fs] = audioread('/Users/Christine/Desktop/audio/Ah_mono.wav');
[cpp,Fs] = audioread('/Users/Christine/Desktop/audio/Ah_mono_cpp.wav');

Modfreq = 5;
Width = 0.001;
mat = vibrato(y,Fs,Modfreq,Width);
audiowrite('/Users/Christine/Desktop/audio/Ah_mono_mat.wav',mat,Fs);

err = immse(cpp(2:end),mat(1:end-1));

%% test2: sweep_stereo.wav
[y,Fs] = audioread('/Users/Christine/Desktop/audio/sweep_stereo.wav');
[cpp,Fs] = audioread('/Users/Christine/Desktop/audio/sweep_stereo_cpp.wav');

Modfreq = 3;
Width = 0.002;
mat1 = vibrato(y(:,1),Fs,Modfreq,Width);
mat2 = vibrato(y(:,2),Fs,Modfreq,Width);
mat = [mat1 mat2];
audiowrite('/Users/Christine/Desktop/audio/sweep_stereo_mat.wav',mat,Fs);

err1 = immse(cpp(2:end,1),mat1(1:end-1));
err2 = immse(cpp(2:end,2),mat2(1:end-1));