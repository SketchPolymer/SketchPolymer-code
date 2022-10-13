onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -expand -group SYSTEM /distri_sketch_top_tb/i1/clk
add wave -noupdate -expand -group SYSTEM /distri_sketch_top_tb/i1/rst
add wave -noupdate -expand -group FLOW -radix decimal /distri_sketch_top_tb/id_number
add wave -noupdate -expand -group FLOW /distri_sketch_top_tb/i1/insert_id_i
add wave -noupdate -expand -group FLOW /distri_sketch_top_tb/i1/insert_latency_i
add wave -noupdate -expand -group FLOW /distri_sketch_top_tb/i1/insert_freq_th_i
add wave -noupdate -expand -group INSERT -expand -group CONTROL /distri_sketch_top_tb/i1/U_insert_control/insert_control_en_i
add wave -noupdate -expand -group INSERT -expand -group CONTROL /distri_sketch_top_tb/i1/U_insert_control/insert_id_i_o
add wave -noupdate -expand -group INSERT -expand -group CONTROL /distri_sketch_top_tb/i1/U_insert_control/insert_latency_o
add wave -noupdate -expand -group INSERT -expand -group CONTROL /distri_sketch_top_tb/i1/U_insert_control/insert_freq_th_o
add wave -noupdate -expand -group INSERT -expand -group CONTROL -expand -group STATE /distri_sketch_top_tb/i1/U_insert_control/current_state
add wave -noupdate -expand -group INSERT -expand -group CONTROL -expand -group STATE /distri_sketch_top_tb/i1/U_insert_control/next_state
add wave -noupdate -expand -group INSERT -expand -group STAGE1 /distri_sketch_top_tb/i1/U_stage1_hash/insert_i
add wave -noupdate -expand -group INSERT -expand -group STAGE1 /distri_sketch_top_tb/i1/U_stage1_hash/hasha_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE1 /distri_sketch_top_tb/i1/U_stage1_hash/hashb_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE1 -radix decimal /distri_sketch_top_tb/i1/U_stage1_hash/rama_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE1 -radix decimal /distri_sketch_top_tb/i1/U_stage1_hash/ramb_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE1 -radix decimal /distri_sketch_top_tb/i1/U_stage1_hash/counter_min
add wave -noupdate -expand -group INSERT -expand -group STAGE2 /distri_sketch_top_tb/i1/U_stage2_hash/insert_i
add wave -noupdate -expand -group INSERT -expand -group STAGE2 /distri_sketch_top_tb/i1/U_stage2_hash/hasha_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE2 /distri_sketch_top_tb/i1/U_stage2_hash/hashb_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE2 -radix decimal /distri_sketch_top_tb/i1/U_stage2_hash/rama_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE2 -radix decimal /distri_sketch_top_tb/i1/U_stage2_hash/ramb_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE2 /distri_sketch_top_tb/i1/U_stage2_hash/ramc_latency_a
add wave -noupdate -expand -group INSERT -expand -group STAGE2 /distri_sketch_top_tb/i1/U_stage2_hash/ramd_latency_a
add wave -noupdate -expand -group INSERT -expand -group STAGE2 -radix decimal /distri_sketch_top_tb/i1/U_stage2_hash/counter_min
add wave -noupdate -expand -group INSERT -expand -group STAGE2 /distri_sketch_top_tb/i1/U_stage2_hash/latency_min
add wave -noupdate -expand -group INSERT -expand -group STAGE3 /distri_sketch_top_tb/i1/U_stage3_hash/insert_i
add wave -noupdate -expand -group INSERT -expand -group STAGE3 /distri_sketch_top_tb/i1/U_stage3_hash/hasha_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE3 /distri_sketch_top_tb/i1/U_stage3_hash/hashb_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE3 -radix decimal /distri_sketch_top_tb/i1/U_stage3_hash/rama_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE3 -radix decimal /distri_sketch_top_tb/i1/U_stage3_hash/ramb_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE3 -radix decimal /distri_sketch_top_tb/i1/U_stage3_hash/counter_min
add wave -noupdate -expand -group INSERT -expand -group STAGE4 /distri_sketch_top_tb/i1/U_stage4_hash/insert_i
add wave -noupdate -expand -group INSERT -expand -group STAGE4 /distri_sketch_top_tb/i1/U_stage4_hash/hasha_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE4 /distri_sketch_top_tb/i1/U_stage4_hash/hashb_a_result
add wave -noupdate -expand -group INSERT -expand -group STAGE4 -radix decimal /distri_sketch_top_tb/i1/U_stage4_hash/rama_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE4 -radix decimal /distri_sketch_top_tb/i1/U_stage4_hash/ramb_counter_a
add wave -noupdate -expand -group INSERT -expand -group STAGE4 -radix decimal /distri_sketch_top_tb/i1/U_stage4_hash/counter_min
add wave -noupdate /distri_sketch_top_tb/i1/U_insert_control/whole_insert_end
add wave -noupdate -expand -group SEARCH /distri_sketch_top_tb/i1/U_search_control/clk
add wave -noupdate -expand -group SEARCH /distri_sketch_top_tb/i1/U_search_control/rst
add wave -noupdate -expand -group SEARCH -expand -group CONTROL /distri_sketch_top_tb/i1/U_search_control/search_control_en_i
add wave -noupdate -expand -group SEARCH -expand -group CONTROL /distri_sketch_top_tb/i1/U_search_control/search_id_i
add wave -noupdate -expand -group SEARCH -expand -group CONTROL /distri_sketch_top_tb/i1/U_search_control/search_quantile_i
add wave -noupdate -expand -group SEARCH -expand -group CONTROL /distri_sketch_top_tb/i1/U_search_control/search_id_o
add wave -noupdate -expand -group SEARCH -expand -group CONTROL /distri_sketch_top_tb/i1/U_search_control/search_lantency_o
add wave -noupdate -expand -group SEARCH -expand -group CONTROL -expand -group STATE /distri_sketch_top_tb/i1/U_search_control/current_state
add wave -noupdate -expand -group SEARCH -expand -group CONTROL -expand -group STATE /distri_sketch_top_tb/i1/U_search_control/next_state
add wave -noupdate -expand -group SEARCH -expand -group STAGE2 /distri_sketch_top_tb/i1/U_search_control/stage2_search
add wave -noupdate -expand -group SEARCH -expand -group STAGE2 /distri_sketch_top_tb/i1/U_search_control/stage2_quantile
add wave -noupdate -expand -group SEARCH -expand -group STAGE2 -radix hexadecimal /distri_sketch_top_tb/i1/U_search_control/stage2_frequency
add wave -noupdate -expand -group SEARCH -expand -group STAGE2 /distri_sketch_top_tb/i1/U_search_control/stage2_latency
add wave -noupdate -expand -group SEARCH -expand -group STAGE2 /distri_sketch_top_tb/i1/U_search_control/m
add wave -noupdate -expand -group SEARCH -expand -group STAGE2 /distri_sketch_top_tb/i1/U_search_control/number_counter
add wave -noupdate -expand -group SEARCH -expand -group STAGE4 /distri_sketch_top_tb/i1/U_search_control/stage4_search
add wave -noupdate -expand -group SEARCH -expand -group STAGE4 /distri_sketch_top_tb/i1/U_search_control/stage4_freq_out
add wave -noupdate -expand -group SEARCH -expand -group STAGE3 /distri_sketch_top_tb/i1/U_search_control/stage3_search
add wave -noupdate -expand -group SEARCH -expand -group STAGE3 /distri_sketch_top_tb/i1/U_search_control/stage3_freq_out
add wave -noupdate -expand -group SEARCH -expand -group STAGE3 /distri_sketch_top_tb/i1/U_search_control/number_counter
add wave -noupdate -expand -group SEARCH -expand -group OUTPUT /distri_sketch_top_tb/i1/U_search_control/w_latency_out
add wave -noupdate -expand -group SEARCH -expand -group OUTPUT /distri_sketch_top_tb/i1/U_search_control/whole_search_end
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {0 ps} 1} {{Cursor 2} {1260403570 ps} 0} {{Cursor 3} {265378937 ps} 0}
quietly wave cursor active 3
configure wave -namecolwidth 186
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 1
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 80
configure wave -griddelta 25
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {261896313 ps} {269666327 ps}
