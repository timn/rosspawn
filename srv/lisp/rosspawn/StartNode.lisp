; Auto-generated. Do not edit!


(in-package rosspawn-srv)


;//! \htmlinclude StartNode-request.msg.html

(defclass <StartNode-request> (ros-message)
  ((node_file_name
    :reader node_file_name-val
    :initarg :node_file_name
    :type string
    :initform ""))
)
(defmethod serialize ((msg <StartNode-request>) ostream)
  "Serializes a message object of type '<StartNode-request>"
  (let ((__ros_str_len (length (slot-value msg 'node_file_name))))
    (write-byte (ldb (byte 8 0) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 8) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 16) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 24) __ros_str_len) ostream))
  (map nil #'(lambda (c) (write-byte (char-code c) ostream)) (slot-value msg 'node_file_name))
)
(defmethod deserialize ((msg <StartNode-request>) istream)
  "Deserializes a message object of type '<StartNode-request>"
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
(defmethod ros-datatype ((msg (eql '<StartNode-request>)))
  "Returns string type for a service object of type '<StartNode-request>"
  "rosspawn/StartNodeRequest")
(defmethod md5sum ((type (eql '<StartNode-request>)))
  "Returns md5sum for a message object of type '<StartNode-request>"
  #x014964366824ce564884dfdaff1faba6)
(defmethod message-definition ((type (eql '<StartNode-request>)))
  "Returns full string definition for message of type '<StartNode-request>"
  (format nil "string node_file_name~%~%"))
(defmethod serialization-length ((msg <StartNode-request>))
  (+ 0
     4 (length (slot-value msg 'node_file_name))
))
(defmethod ros-message-to-list ((msg <StartNode-request>))
  "Converts a ROS message object to a list"
  (list '<StartNode-request>
    (cons ':node_file_name (node_file_name-val msg))
))
;//! \htmlinclude StartNode-response.msg.html

(defclass <StartNode-response> (ros-message)
  ()
)
(defmethod serialize ((msg <StartNode-response>) ostream)
  "Serializes a message object of type '<StartNode-response>"
)
(defmethod deserialize ((msg <StartNode-response>) istream)
  "Deserializes a message object of type '<StartNode-response>"
  msg
)
(defmethod ros-datatype ((msg (eql '<StartNode-response>)))
  "Returns string type for a service object of type '<StartNode-response>"
  "rosspawn/StartNodeResponse")
(defmethod md5sum ((type (eql '<StartNode-response>)))
  "Returns md5sum for a message object of type '<StartNode-response>"
  #x014964366824ce564884dfdaff1faba6)
(defmethod message-definition ((type (eql '<StartNode-response>)))
  "Returns full string definition for message of type '<StartNode-response>"
  (format nil "~%~%"))
(defmethod serialization-length ((msg <StartNode-response>))
  (+ 0
))
(defmethod ros-message-to-list ((msg <StartNode-response>))
  "Converts a ROS message object to a list"
  (list '<StartNode-response>
))
(defmethod service-request-type ((msg (eql 'StartNode)))
  '<StartNode-request>)
(defmethod service-response-type ((msg (eql 'StartNode)))
  '<StartNode-response>)
(defmethod ros-datatype ((msg (eql 'StartNode)))
  "Returns string type for a service object of type '<StartNode>"
  "rosspawn/StartNode")
