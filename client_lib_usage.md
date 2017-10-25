### You can use asio_kcp client_lib in a event-driven framework (recommend)
### Or using in a simple environment do not have a event-driven framework.


<br>
##1. Using asio_kcp_client in a event-driven framework. 
####You should hook a timer for calling the kcp_client.update()
####all code running in your coding thread. This is no other thread.
<br>
You can set event_callback_func or not.

####Using event_callback_func:
  * in class header define a member client_;
```
    kcp_client client_;
```

  * in An event handle:
```
    client_.set_event_callback()
    hook 5milliseconds_timer_handle in your event-driven framework
    client_.connect_async()
```

  * in 5milliseconds_timer_handle
```
    client_.update()
    client_.update will call event_callback_func back (in same thread) when connect succeed or failed
```

  * aftering the success of connection. You can call c.send_msg in your code
  * client_.update() in 5milliseconds_timer_handle will call event_callback_func back (in same thread) when recved some msg or some error 


#### Not using event_callback_func
  * in class header define a member client_;
```
    kcp_client client_;
```

  * in An event handle:
```
    hook 5milliseconds_timer_handle in your event-driven framework
    client_.connect_async()
```

  * in 5milliseconds_timer_handle
```
    client_.update()
    events = client_.grab_events
    events.for_each  handle event
```

  * aftering the success of connection. You can call c.send_msg in your code
  * client_.update() in 5milliseconds_timer_handle will call event_callback_func back (in same thread) when recved some msg or some error 


## 2. Using asio_kcp_client without a event-driven framework.<br>
please using kcp_client_wrap.<br>
kcp_client_wrap is a facade of kcp_client.<br>
This facade is easy to use. <br>
You do not need use it in an event-driven framework such as boost.asio, libevent, cocos2d, or implement by our own.<br>
Please using kcp_client directly if you coding in an event-driven framework. That's more effective.<br>
kcp_client_wrap will create a work thread that control the udp packet sending and recving.<br>
<br>

#### sync using
```
  kcp_client_wrap c;
  c.connect   // this will block until connect succeed or failed.
  while (true)
  {
     do your things && if some msg need send to server
                           c.send_msg
     events = c.grab_events
     handle_events(events)  // your func
     millisecond_sleep(1) if you want.
  }
```

#### You can async the connection stage or event handle stage.

* async the connect stage
```
 kcp_client_wrap c;
 c.connect_async
 while (c.connect_result() == 1)
     millisecond_sleep 1
```

* async connect with callback_func
```
 kcp_client_wrap c;
 c.set_event_callback_func()
 c.connect_async
 ...
 kcp_client_wrap will call event_callback_func in another thread.   note: you should making event_callback_func multithread safe.
```

* async event handle
```
 kcp_client_wrap c;
 c.set_event_callback
 c.connect  or using  c.connect_async mode
 while (true)
 {
     do your things && if some msg need send to server
                           c.send_msg
     millisecond_sleep(1) if you want.
 }
 ...
 kcp_client_wrap will call event_call_back_func in another thread. note: you should making event_call_back_func multithread safe.
```
