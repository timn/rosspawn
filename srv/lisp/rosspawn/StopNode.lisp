; Auto-generated. Do not edit!


(in-package rosspawn-srv)


;//! \htmlinclude StopNode-request.msg.html

(defclass <StopNode-request> (ros-message)
  ((node_file_name
    :reader node_file_name-val
    :initarg :node_file_name
    :type string
    :initform ""))
)
(defmethod serialize ((msg <StopNode-request>) ostream)
  "Serializes a message object of type '<StopNode-request>"
  (let ((__ros_str_len (length (slot-value msg 'node_file_name))))
    (write-byte (ldb (byte 8 0) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 8) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 16) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 24) __ros_str_len) ostream))
  (map nil #'(lambda (c) (write-byte (char-code c) ostream)) (slot-value msg 'node_file_name))
)
(defmethod deserialize ((msg <StopNode-request>) istream)
  "Deserializes a message object of type '<StopNode-request>"
  (let ((__ros_str_len 0))
    (setf (ldb (byte 8 0) __ros_str_len) (read-byte istream))
    (setf (ldb (byte 8 8) __ros_str_len) (read-byte istream))
    (setf (ldb (byte 8 16) __ros_str_len) (read-byte istream))
    (setf (ldb (byte 8 24) __ros_str_len) (read-byte istream))
    (setf (slot-value msg 'node_file_name) (make-string __ros_str_len))
    (dotimes (__ros_str_idx __ros_str_len msg)
      (setf (char (slot-value msg 'node_file_name) __ros_str_idx) (code-char (read-byte istream)))))
  msg
)
(defmethod ros-datatype ((msg (eql '<StopNode-request>)))
  "Returns string type for a service object of type '<StopNode-request>"
  "rosspawn/StopNodeRequest")
(defmethod md5sum ((type (eql '<StopNode-request>)))
  "Returns md5sum for a message object of type '<StopNode-request>"
  #x014964366824ce564884dfdaff1faba6)
(defmethod message-definition ((type (eql '<StopNode-request>)))
  "Returns full string definition for message of type '<StopNode-request>"
  (format nil "string node_file_name~%~%"))
(defmethod serialization-length ((msg <StopNode-request>))
  (+ 0
     4 (length (slot-value msg 'node_file_name))
))
(defmethod ros-message-to-list ((msg <StopNode-request>))
  "Converts a ROS message object to a list"
  (list '<StopNode-request>
    (cons ':node_file_name (node_file_name-val msg))
))
;//! \htmlinclude StopNode-response.msg.html

(defclass <StopNode-response> (ros-message)
  ()
)
(defmethod serialize ((msg <StopNode-response>) ostream)
  "Serializes a message object of type '<StopNode-response>"
)
(defmethod deserialize ((msg <StopNode-response>) istream)
  "Deserializes a message object of type '<StopNode-response>"
  msg
)
(defmethod ros-datatype ((msg (eql '<StopNode-response>)))
  "Returns string type for a service object of type '<StopNode-response>"
  "rosspawn/StopNodeResponse")
(defmethod md5sum ((type (eql '<StopNode-response>)))
  "Returns md5sum for a message object of type '<StopNode-response>"
  #x014964366824ce564884dfdaff1faba6)
(defmethod message-definition ((type (eql '<StopNode-response>)))
  "Returns full string definition for message of type '<StopNode-response>"
  (format nil "~%~%"))
(defmethod serialization-length ((msg <StopNode-response>))
  (+ 0
))
(defmethod ros-message-to-list ((msg <StopNode-response>))
  "Converts a ROS message object to a list"
  (list '<StopNode-response>
))
(defmethod service-request-type ((msg (eql 'StopNode)))
  '<StopNode-request>)
(defmethod service-response-type ((msg (eql 'StopNode)))
  '<StopNode-response>)
(defmethod ros-datatype ((msg (eql 'StopNode)))
  "Returns string type for a service object of type '<StopNode>"
  "rosspawn/StopNode")
