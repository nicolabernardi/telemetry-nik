# Reading messages from CAN bus (socket) and logging to a file   
Launch first ``eagle virtualize can`` and ``eagle simulate can`` from eagle-cli command line tool.    
Then launch ``./read-can`` (already compiled), or if you prefer you can re-compile again.   
CAN messages are logged into the file ``can.log`` with the following format (the same as the log file that eagle-cli uses):   
``(<timestamp_sec>.<timestamp_usec>) <interface> <can_frame_id>#<can_frame_data>``   
Note that in this case (simulation) ``<interface>`` is vcan0    
Errors are logged to console (stderr stream) and to ``err.log`` file with the following format:    
``<timestamp> <error_code> <message>``    
where ``<error_code>`` follows a semantic related to this program, that is:   
1. error on opening file while initializing program   
2. error on binding the CAN socket   
3. error on reading from CAN,   
4. error while reopening the log file (that is periodically closed and reopened to be sure that updates are pushed to disk)   

