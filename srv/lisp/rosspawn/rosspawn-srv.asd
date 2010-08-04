
(in-package :asdf)

(defsystem "rosspawn-srv"
  :default-component-class roslisp-msg-source-file
  :depends-on (:roslisp-msg-protocol :roslisp-utils)
  :components ((:file "_package")
    (:file "StartNode" :depends-on ("_package"))
    (:file "_package_StartNode" :depends-on ("_package"))
    (:file "StopNode" :depends-on ("_package"))
    (:file "_package_StopNode" :depends-on ("_package"))
    (:file "ListLoaded" :depends-on ("_package"))
    (:file "_package_ListLoaded" :depends-on ("_package"))
    ))
