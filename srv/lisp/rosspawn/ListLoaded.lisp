; Auto-generated. Do not edit!


(in-package rosspawn-srv)


;//! \htmlinclude ListLoaded-request.msg.html

(defclass <ListLoaded-request> (ros-message)
  ()
)
(defmethod serialize ((msg <ListLoaded-request>) ostream)
  "Serializes a message object of type '<ListLoaded-request>"
)
(defmethod deserialize ((msg <ListLoaded-request>) istream)
  "Deserializes a message object of type '<ListLoaded-request>"
  msg
)
(defmethod ros-datatype ((msg (eql '<ListLoaded-request>)))
  "Returns string type for a service object of type '<ListLoaded-request>"
  "rosspawn/ListLoadedRequest")
(defmethod md5sum ((type (eql '<ListLoaded-request>)))
  "Returns md5sum for a message object of type '<ListLoaded-request>"
  #x3d07bfda1268b4f76b16b7ba8a82665d)
(defmethod message-definition ((type (eql '<ListLoaded-request>)))
  "Returns full string definition for message of type '<ListLoaded-request>"
  (format nil "~%"))
(defmethod serialization-length ((msg <ListLoaded-request>))
  (+ 0
))
(defmethod ros-message-to-list ((msg <ListLoaded-request>))
  "Converts a ROS message object to a list"
  (list '<ListLoaded-request>
))
;//! \htmlinclude ListLoaded-response.msg.html

(defclass <ListLoaded-response> (ros-message)
  ((nodes
    :reader nodes-val
    :initarg :nodes
    :type (vector string)
   :initform (make-array 0 :element-type 'string :initial-element "")))
)
(defmethod serialize ((msg <ListLoaded-response>) ostream)
  "Serializes a message object of type '<ListLoaded-response>"
  (let ((__ros_arr_len (length (slot-value msg 'nodes))))
    (write-byte (ldb (byte 8 0) __ros_arr_len) ostream)
    (write-byte (ldb (byte 8 8) __ros_arr_len) ostream)
    (write-byte (ldb (byte 8 16) __ros_arr_len) ostream)
    (write-byte (ldb (byte 8 24) __ros_arr_len) ostream))
  (map nil #'(lambda (ele) (let ((__ros_str_len (length ele)))
    (write-byte (ldb (byte 8 0) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 8) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 16) __ros_str_len) ostream)
    (write-byte (ldb (byte 8 24) __ros_str_len) ostream))
  (map nil #'(lambda (c) (write-byte (char-code c) ostream)) ele))
    (slot-value msg 'nodes))
)
(defmethod deserialize ((msg <ListLoaded-response>) istream)
  "Deserializes a message object of type '<ListLoaded-response>"
  (let ((__ros_arr_len 0))
    (setf (ldb (byte 8 0) __ros_arr_len) (read-byte istream))
    (setf (ldb (byte 8 8) __ros_arr_len) (read-byte istream))
    (setf (ldb (byte 8 16) __ros_arr_len) (read-byte istream))
    (setf (ldb (byte 8 24) __ros_arr_len) (read-byte istream))
    (setf (slot-value msg 'nodes) (make-array __ros_arr_len))
    (let ((vals (slot-value msg 'nodes)))
      (dotimes (i __ros_arr_len)
(let ((__ros_str_len 0))
    (setf (ldb (byte 8 0) __ros_str_len) (read-byte istream))
    (setf (ldb (byte 8 8) __ros_str_len) (read-byte istream))
    (setf (ldb (byte 8 16) __ros_str_len) (read-byte istream))
    (setf (ldb (byte 8 24) __ros_str_len) (read-byte istream))
    (setf (aref vals i) (make-string __ros_str_len))
    (dotimes (__ros_str_idx __ros_str_len msg)
      (setf (char (aref vals i) __ros_str_idx) (code-char (read-byte istream))))))))
  msg
)
(defmethod ros-datatype ((msg (eql '<ListLoaded-response>)))
  "Returns string type for a service object of type '<ListLoaded-response>"
  "rosspawn/ListLoadedResponse")
(defmethod md5sum ((type (eql '<ListLoaded-response>)))
  "Returns md5sum for a message object of type '<ListLoaded-response>"
  #x3d07bfda1268b4f76b16b7ba8a82665d)
(defmethod message-definition ((type (eql '<ListLoaded-response>)))
  "Returns full string definition for message of type '<ListLoaded-response>"
  (format nil "string[] nodes~%~%~%"))
(defmethod serialization-length ((msg <ListLoaded-response>))
  (+ 0
     4 (reduce #'+ (slot-value msg 'nodes) :key #'(lambda (ele) (declare (ignorable ele)) (+ 4 (length ele))))
))
(defmethod ros-message-to-list ((msg <ListLoaded-response>))
  "Converts a ROS message object to a list"
  (list '<ListLoaded-response>
    (cons ':nodes (nodes-val msg))
))
(defmethod service-request-type ((msg (eql 'ListLoaded)))
  '<ListLoaded-request>)
(defmethod service-response-type ((msg (eql 'ListLoaded)))
  '<ListLoaded-response>)
(defmethod ros-datatype ((msg (eql 'ListLoaded)))
  "Returns string type for a service object of type '<ListLoaded>"
  "rosspawn/ListLoaded")
