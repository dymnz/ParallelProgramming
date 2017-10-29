clear; close all;

file_name_list = {'2_1.txt', '2_2.txt', '2_4.txt', '2_8.txt', '2_16.txt'};
title_text = '30sec 2opt() call | 1/2/4/8/16-thread';
legend_list = {'1', '2', '4', '6', '8', '16'};

figure;

global_call_min = 9999999999999;
global_call_max = -1;

for i = 1 : length(file_name_list)
file = fileread(file_name_list{i});
call_num_strings = regexp(file, '(?<=call:[^0-9]*)[0-9]+', 'match');
call_num_list = str2double(call_num_strings);

call_min = min(call_num_list);
call_max = max(call_num_list);
call_mean = round(mean(call_num_list));

%%
plot(call_num_list);
hold on;
refline(0, call_mean);

%%
index_min = find(call_min == call_num_list); 
index_max = find(call_max == call_num_list); 

strmin = [num2str(call_min)];
text(index_min, call_min,strmin, 'HorizontalAlignment', 'left', 'FontSize', 15);

strmax = [num2str(call_max)];
text(index_max, call_max, strmax, 'HorizontalAlignment', 'right', 'FontSize', 15);

stravg = [num2str(call_mean)];
text(length(call_num_list), call_mean, stravg, 'HorizontalAlignment', 'right', 'FontSize', 15);

%%
global_call_min = min(global_call_min, call_min);
global_call_max = max(global_call_max, call_max);

disp(sprintf("%s\nMax: %20d\nMin: %20d\nAvg: %20d\n", file_name_list{i}, call_max, call_min, call_mean));
end

xlim([1 100]);
ylim([0.7*global_call_min 1.2*global_call_max]);

hold on;
refline(0, mean(call_num_list));
xlabel('test', 'FontSize', 20);
ylabel('2opt\_swap()', 'FontSize', 20);
title(title_text, 'FontSize', 20);
legend(legend_list);
