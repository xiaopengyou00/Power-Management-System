package com.peng.activity;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import com.peng.slidingmenu.lib.SlidingMenu;
import com.peng.powermanager.R;

import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.*;
import java.lang.*;


public class PoweManager extends Activity {
    private static final String TAG = "PowerManager";
    //按钮控件
    private Button start;
    private Button stop;
    private Button set;
    private Button set_cancel;
    private Button all_start;
    private Button all_stop;
    //开关控件
    private Switch switch_one;
    private Switch switch_two;
    private Switch switch_three;
    //复选按钮控件
    private CheckBox auto_send;
    //文本显示控件
    private TextView re_data_show_one;
    private TextView re_data_show_two;
    private TextView re_data_show_three;
    //照片控件
    private View outside_one;
    private ImageView inside_one;
    private View outside_two;
    private ImageView inside_two;
    private View outside_three;
    private ImageView inside_three;
    //编辑框控件
    private EditText edit_ip;
    private EditText edit_port;
    private EditText edit_time;
    private EditText edit_auto;
    //是否退出
    private boolean Exit;
   // TCP客户端通信模式下
    private TCP_client tcp_client = null;
    private final static int CLIENT_STATE_CORRECT_READ = 7;
    public final static int CLIENT_STATE_CORRECT_WRITE = 8;         //正常通信信息
    public final static int CLIENT_STATE_ERROR = 9;         //发生错误异常信息
    public final static int CLIENT_STATE_IOFO = 10;         //发送SOCKET信息
    //状态信息
    private boolean Client_Link = false;
    private boolean Client_Start = false;
    private boolean Auto_send = false;
    private boolean Switch_one_state_all = false;
    private boolean Switch_two_state_all = false;
    private boolean Switch_three_state_all = false;
    private boolean Set_OK = false;
    private boolean Countdown_Auto = true;
    private boolean Countdown = true;
    //计数信息
    private int re_count = 0;
    private int se_count = 0;

    @SuppressLint("SetTextI18n")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.pm_main);
        SlidingMenuinit();          //侧滑菜单初始化
        init();
        edit_ip.setText(getLocalIpAddress());           //获取本地IP地址显示
        edit_port.setText(888+"");          //设置默认端口号

        start.setOnClickListener(startlistener);
        stop.setOnClickListener(stoplistener);
        set.setOnClickListener(setlistener);
        set_cancel.setOnClickListener(set_cancel_listener);
        auto_send.setOnCheckedChangeListener(autolistener);
        switch_one.setOnCheckedChangeListener(switch_one_listener);
        switch_two.setOnCheckedChangeListener(switch_two_listener);
        switch_three.setOnCheckedChangeListener(switch_three_listener);
        all_start.setOnClickListener(switch_all_start_listener);
        all_stop.setOnClickListener(switch_all_stop_listener);
}
    //初始化控件函数
    private void init() {
        edit_ip = (EditText) findViewById(R.id.ip_edit);
        edit_port = (EditText) findViewById(R.id.port_edit);

        start = (Button) findViewById(R.id.start);
        stop = (Button) findViewById(R.id.stop);
        set = (Button) findViewById(R.id.set_countdowntime);
        set_cancel = (Button) findViewById(R.id.set_cancel);
        all_start = (Button) findViewById(R.id.all_start);
        all_stop = (Button) findViewById(R.id.all_stop);

        auto_send = (CheckBox) findViewById(R.id.auto_send);
        edit_time = (EditText) findViewById(R.id.countdown_time);
        edit_auto = (EditText) findViewById(R.id.edi_auto);

        outside_one = findViewById(R.id.device_one);
        inside_one = (ImageView) findViewById(R.id.light_picture_one);
        outside_two = findViewById(R.id.device_two);
        inside_two = (ImageView) findViewById(R.id.light_picture_two);
        outside_three = findViewById(R.id.device_three);
        inside_three = (ImageView) findViewById(R.id.light_picture_three);

        re_data_show_one = (TextView) findViewById(R.id.data_one);
        re_data_show_one.setMovementMethod(ScrollingMovementMethod
                .getInstance());            // 使TextView接收区可以滚动
        re_data_show_two =  (TextView) findViewById(R.id.data_two);
        re_data_show_one.setMovementMethod(ScrollingMovementMethod
                .getInstance());
        re_data_show_three = (TextView) findViewById(R.id.data_three);
        re_data_show_three.setMovementMethod(ScrollingMovementMethod
                .getInstance());

        switch_one = (Switch) findViewById(R.id.switch_one);
        switch_two = (Switch) findViewById(R.id.switch_two);
        switch_three = (Switch) findViewById(R.id.switch_three);
    }

    //获取wifi本地IP和主机名函数
    @SuppressLint("DefaultLocale")
    private String getLocalIpAddress() {
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
        assert wifiManager != null;
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        // 获取32位整型IP地址
        int ipAddress = wifiInfo.getIpAddress();
        //返回整型地址转换成“*.*.*.*”地址
        return String.format("%d.%d.%d.%d",
                (ipAddress & 0xff), (ipAddress >> 8 & 0xff),
                (ipAddress >> 16 & 0xff), (ipAddress >> 24 & 0xff));
    }

    //开始连接函数
    private View.OnClickListener startlistener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
           if(tcp_client == null) {
               tcp_client = new TCP_client(cli_handler);
               try {
                   InetAddress ipAddress = InetAddress.getByName
                           (edit_ip.getText().toString());
                   int port = Integer.valueOf(edit_port.getText().toString());           //获取端口号
                   tcp_client.setInetAddress(ipAddress);
                   tcp_client.setPort(port);
               } catch (UnknownHostException e) {
                   e.printStackTrace();
               }
               edit_ip.setEnabled(false);
               edit_port.setEnabled(false);
               tcp_client.start();
           }
           start.setEnabled(false);
           stop.setEnabled(true);
            Toast.makeText(PoweManager.this,"开始连接，初始化10s左右"
                    ,Toast.LENGTH_SHORT).show();
        }
    };

    //断开连接函数
    private View.OnClickListener stoplistener= new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if(tcp_client != null){
                tcp_client.close();
                tcp_client=null;
            }
            se_count = 0;
            edit_ip.setEnabled(true);
            edit_port.setEnabled(true);
            start.setEnabled(true);
            stop.setEnabled(false);
            set.setEnabled(true);
            set_cancel.setEnabled(false);
            Toast.makeText(PoweManager.this,"断开连接"
                    ,Toast.LENGTH_SHORT).show();
        }
    };

    //客户端通信函数
    @SuppressLint("HandlerLeak")
    private  Handler cli_handler =new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch( msg.what){
                case CLIENT_STATE_ERROR :
                    Client_Link =false;
                    break;
                case CLIENT_STATE_IOFO :
                    Client_Link =true;
                    Client_Start = true;
                    Toast.makeText(PoweManager.this,"已经连接"
                            ,Toast.LENGTH_SHORT).show();
                    Log.e(TAG,"已经连接");
                    break;
                //接收数据
                case CLIENT_STATE_CORRECT_READ :
                    Handler_receive(msg);
                    break;
                //发送数据
                case CLIENT_STATE_CORRECT_WRITE:
                    if((Countdown_Auto)&&(Set_OK)){
                        Handler_auto_send();
                    }
                    if ((se_count == 1)&&(Countdown)&&(Set_OK)){
                        se_count++;
                        Handler_noauto_send();
                    }
                    break;
            }
        }
    };

    //设置定时时间函数
    private View.OnClickListener setlistener = new View.OnClickListener() {
        char standard1, standard2, standard3;
        @Override
        public void onClick(View v) {
            if(Client_Link){
                String message1 =  edit_time.getText().toString().replaceAll(" ","");
                String message2 =  edit_auto.getText().toString().replaceAll(" ","");
                int length1 = message1.length();
                int length2 = message2.length();
                if ((!Auto_send) && (length1 > 6)){
                    standard1 = message1.charAt(2);
                    standard2 = message1.charAt(5);
                }
                if ((Auto_send) && (length1 > 6) && (length2 > 3)){
                    standard1 = message1.charAt(2);
                    standard2 = message1.charAt(5);
                    standard3 = message2.charAt(2);
                }
                if ((length1 > 7) || (length2 > 4)){
                    if((standard1 != ':')&&(standard2 != ':')){
                        Toast.makeText(PoweManager.this,"填写内容不符合规范，请重新输入！",
                                Toast.LENGTH_SHORT).show();
                    }else if ((Auto_send) && (length2 > 4) && (standard3 != ':')){
                        Toast.makeText(PoweManager.this,"填写内容不符合规范，请重新输入！",
                                Toast.LENGTH_SHORT).show();
                    }else {
                        Set_OK = true;
                        if (!Auto_send){
                            se_count++;
                            Log.e(TAG,"!Auto_send");
                            message1 = 'T' +message1;
                            sendmessage(message1);
                        }else {
                            sendmessage("R");
                            Log.e(TAG,"R");
                        }
                        set.setEnabled(false);
                        set_cancel.setEnabled(true);
                    }
                }else {
                    Toast.makeText(PoweManager.this,"填写内容不符合规范，请重新输入！",
                            Toast.LENGTH_SHORT).show();
                }
            }else{
                Toast.makeText(PoweManager.this,"连接未建立，请稍后...",
                        Toast.LENGTH_SHORT).show();
            }
        }
    };

    //取消定时时间函数
    private View.OnClickListener set_cancel_listener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if(Client_Link){
                se_count = 0;
                Countdown_Auto = true;
                cli_handler.removeCallbacks(runnable);
                set.setEnabled(true);
                set_cancel.setEnabled(false);
            }else{
                Toast.makeText(PoweManager.this,"连接未建立，请稍后...",
                        Toast.LENGTH_SHORT).show();
            }
        }
    };

    //定时传送数据函数
    private Runnable runnable = new Runnable() {
        @Override
        public void run() {
            Countdown_Auto = true;
            Log.e(TAG,"runnable");
            String message = edit_time.getText().toString();
            message = 'T' + message;
            sendmessage(message);
        }
    };

    //定时关闭灯光函数
    private Runnable runnable_lights_off = new Runnable() {
        @Override
        public void run() {
            Countdown = true;
            Log.e(TAG,"runnable_light_off");
            Switch_one_state_all = true; switch_one.setChecked(false);
            Switch_two_state_all = true; switch_two.setChecked(false);
            Switch_three_state_all = true; switch_three.setChecked(false);
        }
    };

    //选择周期函数
    private OnCheckedChangeListener autolistener = new OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            Auto_send = isChecked;
        }
    };

    //屏幕移动函数
    private void SlidingMenuinit(){
        SlidingMenu menu = new SlidingMenu(this);
        menu.setMode(SlidingMenu.LEFT);
        // 设置触摸屏幕的模式
        menu.setTouchModeAbove(SlidingMenu.TOUCHMODE_FULLSCREEN);
        menu.setShadowWidthRes(R.dimen.shadow_width);
        menu.setShadowDrawable(R.drawable.shadow);
        // 设置滑动菜单视图的宽度
        menu.setBehindOffsetRes(R.dimen.slidingmenu_offset);
        // 设置渐入渐出效果的值
        menu.setFadeDegree(0.35f);
        //把滑动菜单添加进所有的Activity中，可选值SLIDING_CONTENT ， SLIDING_WINDOW
        menu.attachToActivity(this, SlidingMenu.SLIDING_CONTENT);
        //为侧滑菜单设置布局
        menu.setMenu(R.layout.pm_setting);
    }

    //发送数据函数
    public void  sendmessage(String message){
        byte[] send = message.getBytes();
        tcp_client.sendmessage(send);
    }

    //周期发送数据处理分析函数，通过handler从子线程回传主线程
    private void Handler_auto_send(){
        if ((Auto_send)) {
            Log.e(TAG,"Handler_auto_send");
            Countdown_Auto = false;
            String Str1 = edit_auto.getText().toString();
            String[] Str_time1 = Str1.split(":");
            String Time_split1 = Str_time1[0] + Str_time1[1];
            char One =Time_split1.charAt(0);
            char Two =Time_split1.charAt(1);
            char Three =Time_split1.charAt(2);
            char Four =Time_split1.charAt(3);
            int Num1 = Integer.parseInt(String.valueOf(One));
            Num1 *= 10 * 3600000;
            int Num2 = Integer.parseInt(String.valueOf(Two));
            Num2 *= 3600000;
            int Num3 = Integer.parseInt(String.valueOf(Three));
            Num3 *= 10 * 60000;
            int Num4 = Integer.parseInt(String.valueOf(Four));
            Num4 *= 60000;
            int Num = Num1 + Num2 + Num3 + Num4;
            String Time1 = String.valueOf(Num);
            long t1 = Long.parseLong(Time1);

            String Str2 = edit_time.getText().toString();
            String[] Str_time2 = Str2.split(":");
            String Time_split2 = Str_time2[0] + Str_time2[1] + Str_time2[2];
            char one =Time_split2.charAt(0);
            char two =Time_split2.charAt(1);
            char three =Time_split2.charAt(2);
            char four =Time_split2.charAt(3);
            char five =Time_split2.charAt(4);
            char six =Time_split2.charAt(5);
            int num1 = Integer.parseInt(String.valueOf(one));
            num1 *= 10 * 3600000;
            int num2 = Integer.parseInt(String.valueOf(two));
            num2 *= 3600000;
            int num3 = Integer.parseInt(String.valueOf(three));
            num3 *= 10 * 60000;
            int num4 = Integer.parseInt(String.valueOf(four));
            num4 *= 60000;
            int num5 = Integer.parseInt(String.valueOf(five));
            num5 *= 10 * 1000;
            int num6 = Integer.parseInt(String.valueOf(six));
            num6 *= 1000;
            int num = num1 + num2 + num3 + num4 +num5 + num6 +Num;
            String Time2 = String.valueOf(num);
            long t2 = Long.parseLong(Time2);

            cli_handler.postDelayed(runnable, t1);
            cli_handler.postDelayed(runnable_lights_off, t2);
        } else {
            cli_handler.removeCallbacks(runnable);
        }
    }

    //不是周期发送数据处理分析函数，通过handler从子线程回传主线程
    private void Handler_noauto_send(){
        if ((!Auto_send)) {
            Countdown = false;
            String Str = edit_time.getText().toString();
            String[] Str_time = Str.split(":");
            String Time_split = Str_time[0] + Str_time[1] + Str_time[2];
            char one =Time_split.charAt(0);
            char two =Time_split.charAt(1);
            char three =Time_split.charAt(2);
            char four =Time_split.charAt(3);
            char five =Time_split.charAt(4);
            char six =Time_split.charAt(5);
            int num1 = Integer.parseInt(String.valueOf(one));
            num1 *= 10 * 3600000;
            int num2 = Integer.parseInt(String.valueOf(two));
            num2 *= 3600000;
            int num3 = Integer.parseInt(String.valueOf(three));
            num3 *= 10 * 60000;
            int num4 = Integer.parseInt(String.valueOf(four));
            num4 *= 60000;
            int num5 = Integer.parseInt(String.valueOf(five));
            num5 *= 10 * 1000;
            int num6 = Integer.parseInt(String.valueOf(six));
            num6 *= 1000;
            int num = num1 + num2 + num3 + num4 +num5 + num6 ;
            String Time = String.valueOf(num);
            long t = Long.parseLong(Time);

            cli_handler.postDelayed(runnable_lights_off, t);
        } else {
            cli_handler.removeCallbacks(runnable);
        }
    }

    //按钮一函数
    private CompoundButton.OnCheckedChangeListener switch_one_listener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
            if (Client_Link) {
                if ((Switch_one_state_all)){
                    Switch_one_state_all = false;
                    if (isChecked) {
                        inside_one.setImageResource(R.drawable.light_on_24dp);
                        outside_one.setBackgroundResource(R.drawable.adapter_control_light_on);
                    }else{
                        inside_one.setImageResource(R.drawable.light_off_24dp);
                        outside_one.setBackgroundResource(R.drawable.adapter_control_light_off);
                    }
                }else {
                    if (isChecked) {
                        String message = "ON1";
                        sendmessage(message);
                        inside_one.setImageResource(R.drawable.light_on_24dp);
                        outside_one.setBackgroundResource(R.drawable.adapter_control_light_on);
                    }else{
                        String message = "OFF1";
                        sendmessage(message);
                        inside_one.setImageResource(R.drawable.light_off_24dp);
                        outside_one.setBackgroundResource(R.drawable.adapter_control_light_off);
                    }
                }
            }else{
                Toast.makeText(PoweManager.this, "连接未建立，请稍后...",
                        Toast.LENGTH_SHORT).show();
            }
        }
    };

    //按钮二函数
    private CompoundButton.OnCheckedChangeListener switch_two_listener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
            if (Client_Link) {
                if (Switch_two_state_all){
                    Switch_two_state_all = false;
                    if (isChecked) {
                        inside_two.setImageResource(R.drawable.light_on_24dp);
                        outside_two.setBackgroundResource(R.drawable.adapter_control_light_on);
                    }else{
                        inside_two.setImageResource(R.drawable.light_off_24dp);
                        outside_two.setBackgroundResource(R.drawable.adapter_control_light_off);
                    }
                }else {
                    if (isChecked) {
                        String message =  "ON2";
                        sendmessage(message);
                        inside_two.setImageResource(R.drawable.light_on_24dp);
                        outside_two.setBackgroundResource(R.drawable.adapter_control_light_on);
                    }else{
                        String message =  "OFF2";
                        sendmessage(message);
                        inside_two.setImageResource(R.drawable.light_off_24dp);
                        outside_two.setBackgroundResource(R.drawable.adapter_control_light_off);
                    }
                }
            }else{
                Toast.makeText(PoweManager.this,"连接未建立，请稍后...",
                        Toast.LENGTH_SHORT).show();
            }
        }
    };

    //按钮三函数
    private CompoundButton.OnCheckedChangeListener switch_three_listener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
            if (Client_Link) {
                if (isChecked) {
                    if (Switch_three_state_all) {
                        Switch_three_state_all = false;
                        inside_three.setImageResource(R.drawable.light_on_24dp);
                        outside_three.setBackgroundResource(R.drawable.adapter_control_light_on);
                    }else{
                        String message =  "ON3";
                        sendmessage(message);
                        inside_three.setImageResource(R.drawable.light_on_24dp);
                        outside_three.setBackgroundResource(R.drawable.adapter_control_light_on);
                    }
                }else {
                    if (Switch_three_state_all) {
                        Switch_three_state_all = false;
                        inside_three.setImageResource(R.drawable.light_off_24dp);
                        outside_three.setBackgroundResource(R.drawable.adapter_control_light_off);
                    }else{
                        String message =  "OFF3";
                        sendmessage(message);
                        inside_three.setImageResource(R.drawable.light_off_24dp);
                        outside_three.setBackgroundResource(R.drawable.adapter_control_light_off);
                    }
                }
            }else{
                Toast.makeText(PoweManager.this,"连接未建立，请稍后...",
                        Toast.LENGTH_SHORT).show();
            }
        }
    };

    //全开函数
    private View.OnClickListener switch_all_start_listener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if(Client_Link){
                String message =  "ON_ALL";
                sendmessage(message);
                Switch_one_state_all = true; switch_one.setChecked(true);
                Switch_two_state_all = true; switch_two.setChecked(true);
                Switch_three_state_all = true; switch_three.setChecked(true);
            }else{
                Toast.makeText(PoweManager.this,"连接未建立，请稍后...",
                        Toast.LENGTH_SHORT).show();
            }
        }
    };

    //全关函数
    private View.OnClickListener switch_all_stop_listener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if(Client_Link){
                String message =  "OFF_ALL";
                sendmessage(message);
                Switch_one_state_all = true; switch_one.setChecked(false);
                Switch_two_state_all = true; switch_two.setChecked(false);
                Switch_three_state_all = true; switch_three.setChecked(false);
            }else{
                Toast.makeText(PoweManager.this,"连接未建立，请稍后...",
                        Toast.LENGTH_SHORT).show();
            }
        }
    };

    // 接收数据处理分析函数，通过handler从子线程回传到主线程
    private  void Handler_receive(Message msg){
        byte[]  buffer= (byte[]) msg.obj;
        String readMessage = null;
        try {
            readMessage = new String(buffer, 0, msg.arg1, "GBK");
            re_count++;
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        @SuppressLint("SimpleDateFormat")
        SimpleDateFormat formatter = new SimpleDateFormat("yyyy年MM月dd日   HH:mm:ss");
        Date curDate =new Date(System.currentTimeMillis());
        String time = formatter.format(curDate);            //获取当前时间

        assert readMessage != null;
        char re_state1 = readMessage.charAt(readMessage.length() - 3);
        char re_state2 = readMessage.charAt(readMessage.length() - 2);
        char re_state3 = readMessage.charAt(readMessage.length() - 1);

        if (Client_Link){
            if (Client_Start){          //监听一开始连接上的电源状态
                Client_Start = false;
                if (re_state1 == '0') {
                    switch_one.setChecked(true);
                }else {
                    switch_one.setChecked(false);
                }
                if (re_state2 == '0') {
                    switch_two.setChecked(true);
                }else {
                    switch_two.setChecked(false);
                }
                if (re_state3 == '0') {
                    switch_three.setChecked(true);
                }else {
                    switch_three.setChecked(false);
                }
            }

            if (re_count >= 5){         //每10s输出数据一次
                String readMessage1 = readMessage.substring(readMessage.indexOf("Z")+1, readMessage.indexOf("z"));
                readMessage1 = time + "\n" + readMessage1 + "\n";           //对接收到的数据进行分析处理
                re_data_show_one.append(readMessage1);
                int offset1 = re_data_show_one.getLineCount() * re_data_show_one.getLineHeight();           //使textview滚动到最底层
                if(offset1 > re_data_show_one.getHeight()){
                    re_data_show_one.scrollTo(0,offset1 - re_data_show_one.getHeight());
                }

                String readMessage2 = readMessage.substring(readMessage.indexOf("Y")+1, readMessage.indexOf("y"));
                readMessage2 = time + "\n" + readMessage2 + "\n";           //对接收到的数据进行分析处理
                re_data_show_two.append(readMessage2);
                int offset2 = re_data_show_two.getLineCount() * re_data_show_two.getLineHeight();           //使textview滚动到最底层
                if(offset2 > re_data_show_two.getHeight()){
                    re_data_show_two.scrollTo(0,offset2 - re_data_show_two.getHeight());
                }

                String readMessage3 = readMessage.substring(readMessage.indexOf("X")+1, readMessage.indexOf("x"));
                readMessage3 = time + "\n" + readMessage3 + "\n";           //对接收到的数据进行分析处理
                re_data_show_three.append(readMessage3);
                int offset3 = re_data_show_three.getLineCount() * re_data_show_three.getLineHeight();           //使textview滚动到最底层
                if(offset3 > re_data_show_one.getHeight()){
                    re_data_show_three.scrollTo(0,offset3 - re_data_show_three.getHeight());
                }

                re_count = 0;
            }
        }
    }

    @Override
    public void onBackPressed() {
        exit();
    }

    //页面退出函数
    public void exit(){
        if(Exit){
            this.finish();
        }
        Exit = true;
        Toast.makeText(this,"再按一次，返回桌面",Toast.LENGTH_SHORT).show();
    }
}
