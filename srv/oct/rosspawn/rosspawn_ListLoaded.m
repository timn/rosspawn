% Auto-generated.  Do not edit!

% [reqmsg,resmsg] = core_testsrv1()
%
function [reqmsg,resmsg] = rosspawn_ListLoaded()
if( nargout > 0 )
    reqmsg = rosspawn_Request();
end
if( nargout > 0 )
    resmsg = rosspawn_Response();
end

% Auto-generated.  Do not edit!

% msg = rosspawn_Request()
%
% Request message type, fields include:

% //! \htmlinclude Request.msg.html
function msg = rosspawn_Request()

msg = [];
msg.create_response_ = @rosspawn_Response;
msg.md5sum_ = @rosspawn_Request___md5sum;
msg.server_md5sum_ = @rosspawn_Request___server_md5sum;
msg.server_type_ = @rosspawn_Request___server_type;
msg.type_ = @rosspawn_Request___type;
msg.serializationLength_ = @rosspawn_Request___serializationLength;
msg.serialize_ = @rosspawn_Request___serialize;
msg.deserialize_ = @rosspawn_Request___deserialize;
msg.message_definition_ = @rosspawn_Request___message_definition;

function x = rosspawn_Request___md5sum()
x = '';

function x = rosspawn_Request___server_md5sum()
x = '3d07bfda1268b4f76b16b7ba8a82665d';

function x = rosspawn_Request___server_type()
x = '';

function x = rosspawn_Request___message_definition()
x = [    '\n' ...
];

function x = rosspawn_Request___type()
x = 'rosspawn/ListLoadedRequest';

function l__ = rosspawn_Request___serializationLength(msg)
l__ = 0;

function dat__ = rosspawn_Request___serialize(msg__, seq__, fid__)
global rosoct

c__ = 0;
file_created__ = 0;
if( ~exist('fid__','var') )
    fid__ = tmpfile();
    file_created__ = 1;
end
if( file_created__ )
    fseek(fid__,0,SEEK_SET);
    dat__ = fread(fid__,Inf,'uint8=>uint8');
    fclose(fid__);
end

function msg__ = rosspawn_Request___deserialize(dat__, fid__)
msg__ = rosspawn_Request();
file_created__ = 0;
if( ~exist('fid__','var') )
    fid__ = tmpfile();
    file_created__ = 1;
    fwrite(fid__,dat__,'uint8');
    fseek(fid__,0,SEEK_SET);
end
if( file_created__ )
    fclose(fid__);
end
function l__ = rosspawn_Request___sum_array_length__(x)
if( ~exist('x','var') || isempty(x) )
    l__ = 0;
else
    l__ = sum(x(:));
end

% msg = rosspawn_Response()
%
% Response message type, fields include:
% string{} nodes

% //! \htmlinclude Response.msg.html
function msg = rosspawn_Response()

msg = [];
msg.nodes = {};
msg.md5sum_ = @rosspawn_Response___md5sum;
msg.server_md5sum_ = @rosspawn_Response___server_md5sum;
msg.server_type_ = @rosspawn_Response___server_type;
msg.type_ = @rosspawn_Response___type;
msg.serializationLength_ = @rosspawn_Response___serializationLength;
msg.serialize_ = @rosspawn_Response___serialize;
msg.deserialize_ = @rosspawn_Response___deserialize;
msg.message_definition_ = @rosspawn_Response___message_definition;

function x = rosspawn_Response___md5sum()
x = '';

function x = rosspawn_Response___server_md5sum()
x = '3d07bfda1268b4f76b16b7ba8a82665d';

function x = rosspawn_Response___server_type()
x = '';

function x = rosspawn_Response___message_definition()
x = [    '\n' ...
];

function x = rosspawn_Response___type()
x = 'rosspawn/ListLoadedResponse';

function l__ = rosspawn_Response___serializationLength(msg)
l__ =  ...
    + 4 + rosspawn_Response___sum_array_length__(cellfun(@(nodes_elt__) 4 + numel(nodes_elt__), msg.nodes));

function dat__ = rosspawn_Response___serialize(msg__, seq__, fid__)
global rosoct

c__ = 0;
file_created__ = 0;
if( ~exist('fid__','var') )
    fid__ = tmpfile();
    file_created__ = 1;
end
fwrite(fid__, numel(msg__.nodes), 'uint32');
for nodes_i__ = 1:numel(msg__.nodes)
    fwrite(fid__, numel(msg__.nodes{nodes_i__}), 'uint32');
fwrite(fid__, msg__.nodes{nodes_i__}, 'uint8');
end
if( file_created__ )
    fseek(fid__,0,SEEK_SET);
    dat__ = fread(fid__,Inf,'uint8=>uint8');
    fclose(fid__);
end

function msg__ = rosspawn_Response___deserialize(dat__, fid__)
msg__ = rosspawn_Response();
file_created__ = 0;
if( ~exist('fid__','var') )
    fid__ = tmpfile();
    file_created__ = 1;
    fwrite(fid__,dat__,'uint8');
    fseek(fid__,0,SEEK_SET);
end
size__ = double(fread(fid__, 1, 'uint32=>uint32'));
msg__.nodes = cell(size__, 1);
for nodes_i__ = 1:size__
    size__ = double(fread(fid__, 1,'uint32=>uint32'));
msg__.nodes{nodes_i__} = fread(fid__, size__, '*char')';
end
if( file_created__ )
    fclose(fid__);
end
function l__ = rosspawn_Response___sum_array_length__(x)
if( ~exist('x','var') || isempty(x) )
    l__ = 0;
else
    l__ = sum(x(:));
end

