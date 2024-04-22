function combined_gui()
    % Create the main figure
    fig = figure('Name', 'Combined GUI', 'NumberTitle', 'off', ...
        'Position', [100, 100, 400, 500], 'MenuBar', 'none', ...
        'ToolBar', 'none', 'Resize', 'off');

    % Create an input field to display the pressed digits
    input_field = uicontrol('Style', 'edit', 'Position', [20, 450, 360, 30]);

    % Create buttons for digits 0-9
    for digit = 1:9
        uicontrol('Style', 'pushbutton', 'String', num2str(digit), ...
            'Position', [20 + 90 * mod(digit-1, 3), 350 - 70 * floor((digit-1) / 3), 70, 70], ...
            'Callback', @(~,~) update_input_field(input_field, digit));
    end

    uicontrol('Style', 'pushbutton', 'String', num2str("*"), ...
            'Position', [20 + 90 * mod(9, 3), 350 - 70 * floor(9 / 3), 70, 70], ...
            'Callback', @(~,~) update_input_field(input_field, "*"));

    uicontrol('Style', 'pushbutton', 'String', num2str(0), ...
            'Position', [20 + 90 * mod(10, 3), 350 - 70 * floor(10 / 3), 70, 70], ...
            'Callback', @(~,~) update_input_field(input_field, 0));

    uicontrol('Style', 'pushbutton', 'String', num2str("#"), ...
            'Position', [20 + 90 * mod(11, 3), 350 - 70 * floor(11 / 3), 70, 70], ...
            'Callback', @(~,~) update_input_field(input_field, "#"));

    % Create a clear button
    uicontrol('Style', 'pushbutton', 'String', 'Clear', ...
        'Position', [20, 20, 160, 50], 'Callback', @(~,~) clear_input_field(input_field));

    % Create a send button for sending text to PSoC
    uicontrol('Style', 'pushbutton', 'String', 'Send to PSoC', ...
        'Position', [220, 20, 160, 50], 'Callback', @(~,~) send_to_psoc(input_field));

    % Serial port configuration
    s = serialport('COM7', 9600);
    configureTerminator(s, "LF");

    % Callback function to send text to PSoC
    function send_to_psoc(input_field)
        % Get the text from the input field

        for i=1:5
            text = get(input_field, 'String');
        % Send text to PSoc
            write(s, text, "string");
        end
        
        % Update status label
        disp(['Text sent to PSoC: ' text]);
    end

    % Callback function to update the input field with pressed digits
    function update_input_field(input_field, digit)
        current_text = get(input_field, 'String');
        new_text = [current_text num2str(digit)];
        set(input_field, 'String', new_text);
    end

    % Callback function to clear the input field
    function clear_input_field(input_field)
        set(input_field, 'String', '');
    end
end


