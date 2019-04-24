function client()
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is hardcoded.
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
port='/dev/tty.usbserial-DM01NMGC';
fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out

mySerial = serial(port, 'BaudRate', 230400); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf('a:  Read current sensor(ADC counts)')  %b: Read current sensor (mA)\n   c: Read encoder counts  d: Read encoder\n   e: Reset encoder f:set PWM(-100 to 100)\n  g: Set current gains  h: Get current gains\n  i: Set position gains j: Get position gains\n   k: Test current control   l:Go to angle(deg)\n     m:Load step trajectory  n:Load cubic trajectory\n    o:Execute trajectory  p: Unpower the motor \n  q: Quit\n     x: Add  r:Get mode ');
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);
    
    % take the appropriate action
    switch selection
        case 'a'                       %read adc count
             
             %fprintf(mySerial, 'a\n');
            adc = fscanf(mySerial, '%d %d\n');
            fprintf('curent %d  count %d.' , adc)
         case 'b'                       %read adc current mA
             
             %fprintf(mySerial, 'a\n');
            current = fscanf(mySerial, '%d');
            fprintf('The current is %d mA.' , current)
        case 'd'                         % get degree
             
             %fprintf(mySerial, 'd\n');
            deg = fscanf(mySerial, '%d');
            fprintf('The motor angle is %d degree.' , deg)
         case 'f'                         % set PWM 100 to -100
             
             a = input('Enter the PWM (-100 to 100): ');
             fprintf(mySerial, '%d\n', a);
            
            fprintf('PWM set.')
        case 'g' %set current gain
            
            gain = input('Enter gain kp ki:');
            fprintf(mySerial,'%f %f\n',gain);
            fprintf('%f %f set', gain);

            
        case 'h' %get gain 
            gain = fscanf(mySerial, '%f %f\n');
            fprintf('kp%f ki%f set',gain);
            
         case 'i' %set postion gain
            
            pgain = input('Enter position kp ki kd:');
            fprintf(mySerial,'%f %f %f\n',pgain);
            fprintf('%f %f %fset', pgain);

            
        case 'j' %get position gain 
            pgain = fscanf(mySerial, '%f %f %f\n');
            fprintf('kpi%f ki%f kd%f set',pgain);
         
        case 'l' %go to angle
            desired_angle = input('Enter angle:');
            fprintf(mySerial,'%d',desired_angle);
        case 'p'
            fprintf('unpowered.')
        case 'k'
            
             read_plot_matrix(mySerial)
            
        case'r'
            %fprintf(mySerial, 'r\n');
            mode = fscanf(mySerial, '%d');
            switch mode
                case 0
                    fprintf('IDLE Mode.')
                case 1
                    fprintf('PWM Mode.')
                case 2
                    fprintf('ITEST Mode.')
                case 3
                    fprintf('HOLD Mode.')
                case 4
                    fprintf('TRACK Mode.')
                otherwise
                    fprintf('Wrong.')
                      
           end
               
            
        case 'q'
            has_quit = true;             % exit client
            
        case 'c'
             
             
            counts1 = fscanf(mySerial, '%d');
           
            fprintf('The motor angle is %d counts.' , counts1);
        case 'e'
            
            
            counts2 = fscanf(mySerial, '%d');
            fprintf('The motor angle is %d counts.' , counts2)
        case 'm'
            reflist=input('Enter step trajectory');
            ref=genRef(reflist, 'step');
           
            fprintf(mySerial, '%d\n', length(ref));
            for c = 1:length(ref)
                fprintf(mySerial, '%d\n', ref(c));
            
            end
            
            
          
        case 'n'
            reflist2=input('Enter cubic trajectory');
            ref2=genRef(reflist2, 'cubic');
            fprintf(mySerial, '%d\n', length(ref2));
            for d = 1:length(ref2)
                a=round(ref2(d));
                fprintf(mySerial, '%d\n', a);
            end
        
        
            
        case 'x'                         % example operation
            a = input('Enter the First number: '); % get the number to send
            b = input('Enter the Second number: ');
            A=[a;b];
            
            fprintf(mySerial, '%d %d\n', A); % send the number
            n = fscanf(mySerial,'%d');   % get the added number
            fprintf('Read: %d\n',n);     % print it to the screen
               
        case 'o'
            read_plot_matrix(mySerial);
            fprintf('Trajectory'); 
            
            
      
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
