clear; close all;

file_name_list = {'2_1.txt', '2_2.txt', '2_4.txt', '21_8.txt', '2_16.txt'};
title_text = '30sec 2opt() call | 1/2/4/8/16-thread';
thread_legend_list = {'1', '2', '4', '8', '16'};
avg_legend_list = {'avg-1', 'avg-2', 'avg-4', 'avg-8', 'avg-16'};

figure;

global_call_min = 9999999999999;
global_call_max = -1;

max_list = zeros(1, length(file_name_list));
min_list = zeros(1, length(file_name_list));
mean_list = zeros(1, length(file_name_list));

for i = 1 : length(file_name_list)
file = fileread(file_name_list{i});
call_num_strings = regexp(file, '(?<=call:[^0-9]*)[0-9]+', 'match');
call_num_list = str2double(call_num_strings);

call_min = min(call_num_list);
call_max = max(call_num_list);
call_mean = round(mean(call_num_list));

max_list(i) = call_max;
min_list(i) = call_min;
mean_list(i) = call_mean;

%%
scatter((log2(str2num(thread_legend_list{i})))*ones(1, length(call_num_list)), call_num_list);
hold on;

%%
global_call_min = min(global_call_min, call_min);
global_call_max = max(global_call_max, call_max);

disp(sprintf("%s\nMax: %20d\nMin: %20d\nAvg: %20d\n", file_name_list{i}, call_max, call_min, call_mean));
end
%%
xlim([0 4]);
xticks([0 1 2 3 4]);
xticklabels({'1', '2', '4', '8', '16'});
ylim([0.7*global_call_min 1.2*global_call_max]);

xt = get(gca, 'XTick');
set (gca, 'XTickLabel', 2.^xt);

xlabel('Thread(s)', 'FontSize', 20);
ylabel('2opt\_swap()', 'FontSize', 20);
title(title_text, 'FontSize', 20);

%%
for i = 1 : length(file_name_list)
    %refline(0, mean_list(i));
    strmax = [num2str(max_list(i))];
    strmin = [num2str(min_list(i))];
    strmean = [num2str(mean_list(i))];
    text(log2(str2num(thread_legend_list{i}))+0.3, 1.1 * max_list(i), strmax, 'HorizontalAlignment', 'right', 'FontSize', 15);
    text(log2(str2num(thread_legend_list{i}))+0.3, 0.9 * min_list(i), strmin, 'HorizontalAlignment', 'right', 'FontSize', 15);
    text(log2(str2num(thread_legend_list{i}))+0.3, mean_list(i), strmean, 'HorizontalAlignment', 'right', 'FontSize', 15);
    scatter((log2(str2num(thread_legend_list{i}))), max_list(i), 100, 'red', 'x');
    scatter((log2(str2num(thread_legend_list{i}))), min_list(i), 100, 'red', 'x');
    scatter((log2(str2num(thread_legend_list{i}))), mean_list(i), 100, 'red', 'x');
end
legend(thread_legend_list);
%legend(avg_legend_list);