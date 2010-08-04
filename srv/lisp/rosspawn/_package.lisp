(defpackage rosspawn-srv
  (:use cl
        roslisp-msg-protocol)
  (:export
   "STARTNODE"
   "<STARTNODE-REQUEST>"
   "<STARTNODE-RESPONSE>"
   "STOPNODE"
   "<STOPNODE-REQUEST>"
   "<STOPNODE-RESPONSE>"
   "LISTLOADED"
   "<LISTLOADED-REQUEST>"
   "<LISTLOADED-RESPONSE>"
  ))

