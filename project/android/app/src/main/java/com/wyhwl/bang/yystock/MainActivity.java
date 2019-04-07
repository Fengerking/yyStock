package com.wyhwl.bang.yystock;

import android.Manifest;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.inputmethod.InputMethodManager;

import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Toast;
import android.widget.ListView;
import android.widget.BaseAdapter;
import android.widget.TextView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.AdapterView;

import java.io.File;
import java.io.FileInputStream;
import java.util.ArrayList;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.zhy.http.okhttp.OkHttpUtils;
import com.zhy.http.okhttp.callback.FileCallBack;
import com.zhy.http.okhttp.callback.StringCallback;
import okhttp3.Call;
import okhttp3.Request;

public class MainActivity extends AppCompatActivity
        implements View.OnClickListener {
    public static final int     REQUEST_WRITE_EXTERNAL_STORAGE = 1;
    public static final int     MSG_HIDE_INPUT      = 1000;
    public static final int     MSG_UPDATE_INFO     = 1001;
    public static final int     MSG_PARSE_RESULT    = 1002;
    public static final int     MSG_PARSE_ONE       = 1003;

    private msgHandler          m_msgHandler    = null;
    private boolean             m_bMessage = false;
    private boolean             m_bResume = false;
    private Context             m_context = null;
    private AdapterView.OnItemClickListener m_lvListener = null;
    private ListView            m_lstStock = null;
    private EditText            m_edtStock = null;
    private Button              m_btnAdd = null;
    private Button              m_btnDel = null;
    private Button              m_btnUpdate = null;
    private Button              m_btnHide = null;
    private ImageView           m_imgStock = null;
    private ListView            m_lstOne = null;

    private String              m_szCode[] = {"0000001","0600023","0600789","0600577","0601216","0600895"};
    private ArrayList<String>   m_lstCode = null;
    private String[]            m_strInfo = null;
    private int[]               m_nColor = null;
    private String              m_strResponse = null;
    private String              m_strUpdateTime = null;
    private String              m_strStockOne = null;
    private String              m_strStockCode = null;
    private JSONObject          m_jsnItem = null;

    private int                 m_nClrWhite = 0;
    private int                 m_nClrBlack = 0;
    private int                 m_nUpdateTimes = 0;

    private ArrayList<String>   m_lstHistory = null;
    private long                m_lPrevvolume = 0;
    private String              m_strPrevTime = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        m_context = this;
        m_lstStock = (ListView)findViewById(R.id.lstStock);
        m_lvListener = new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                if (position >= m_lstCode.size())
                    return;
                String strCode = m_lstCode.get(position);
                m_edtStock.setText(strCode);
            }
        };
        m_lstStock.setOnItemClickListener(m_lvListener);
        m_edtStock = (EditText)findViewById(R.id.edt_one);
        m_btnAdd = (Button)findViewById(R.id.btn_add);
        m_btnAdd.setOnClickListener(this);
        m_btnDel = (Button)findViewById(R.id.btn_del);
        m_btnDel.setOnClickListener(this);
        m_btnUpdate = (Button)findViewById(R.id.btn_update);
        m_btnUpdate.setOnClickListener(this);
        m_btnHide = (Button)findViewById(R.id.btn_hide);
        m_btnHide.setOnClickListener(this);
        m_imgStock = (ImageView)findViewById(R.id.img_stock);
        m_lstOne = (ListView)findViewById(R.id.lst_one);

        SharedPreferences settings = m_context.getSharedPreferences("User_Setting", 0);
        String strCode = settings.getString("selectStock", null);
        m_lstCode = new ArrayList<String>();
        if (strCode == null) {
            for (int i = 0; i < m_szCode.length; i++)
                m_lstCode.add (m_szCode[i]);
        } else {
            String  strNext = null;
            int     nNext = strCode.indexOf(',');
            while (nNext > 0) {
                strNext = strCode.substring(1, nNext);
                m_lstCode.add(strNext);
                strCode = strCode.substring(nNext + 1);
                nNext = strCode.indexOf(',');
                if (nNext < 0)
                    nNext = strCode.indexOf(']');
            }
        }
        m_lstHistory = new ArrayList<String>();

        m_nClrWhite = Color.rgb(160, 160,160);
        m_nClrBlack = Color.rgb(0, 0,0);
        CheckWritePermission (true);

        m_msgHandler = new msgHandler ();
        m_msgHandler.sendEmptyMessageDelayed(MSG_HIDE_INPUT, 1000);
    }

    protected void onPause() {
        super.onPause();
        m_bResume = false;
    }
    protected void onResume() {
        super.onResume();
        m_bResume = true;
        if (!m_bMessage) {
            m_bMessage = true;
            m_msgHandler.sendEmptyMessageDelayed(MSG_UPDATE_INFO, 2);
        }
    }
    protected void onStop(){
        super.onStop();
        String strCode = m_lstCode.toString();
        SharedPreferences settings = m_context.getSharedPreferences("User_Setting", 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString("selectStock", strCode);
        editor.commit();
    }

    public void onClick(View v){
        int nID = v.getId();
        switch (nID) {
            case R.id.btn_add:
                if (m_strStockCode.length() != 7)
                    break;
                if (!OneInList(m_strStockCode)) {
                    m_lstCode.add (m_strStockCode);
                }
                break;
            case R.id.btn_del:
                if (m_strStockCode.length() != 7)
                    break;
                m_lstCode.remove (m_strStockCode);
                break;

            case R.id.btn_hide:
                m_lstOne.setVisibility(View.INVISIBLE);
                m_imgStock.setVisibility(View.INVISIBLE);
                m_strStockCode = "";
                m_nUpdateTimes = 0;
                m_edtStock.setText("");
                break;

            default:
                break;
        }
    }

    private void UpdateStockList () {
        String strURL = "https://api.money.126.net/data/feed/";
        for (int i = 0; i < m_lstCode.size(); i++) {
            strURL += m_lstCode.get(i); strURL += ",";
        }
        OkHttpUtils
                .get().url(strURL).id(101)
                .build().execute(new httpDataCallBack());

        String strOne = m_edtStock.getText().toString();
        if (strOne.length() == 6) {
            if (strOne.charAt(0) == '6')
                strOne = "0" + strOne;
            else if (strOne.charAt(0) == '3')
                strOne = "1" + strOne;
           else if (strOne.charAt(0) == '0' && strOne.charAt(1) == '0')
                strOne = "1" + strOne;
        }
        if (strOne.length() == 7) {
            m_lstOne.setVisibility(View.VISIBLE);
            m_imgStock.setVisibility(View.VISIBLE);

            if (m_strStockCode != null && m_strStockCode.compareTo(strOne) != 0) {
                m_nUpdateTimes = 0;
                m_strPrevTime = "";
                m_lPrevvolume = 0;
                m_lstHistory.clear();
                m_jsnItem = null;
            }
            m_strStockCode = strOne;

            if (!OneInList(m_strStockCode)) {
                strURL = "https://api.money.126.net/data/feed/";
                strURL += m_strStockCode;
                OkHttpUtils
                        .get().url(strURL).id(201)
                        .build().execute(new httpDataCallBack());
            }

            m_nUpdateTimes++;
            if (m_nUpdateTimes % 10 != 1)
                return;

            strURL = "https://img1.money.126.net/chart/hs/time/540x360/";
            strURL = strURL + m_strStockCode + ".png";
            String strPath = "/sdcard/yyStock00/";
            String strFile = strOne + ".png";
            OkHttpUtils
                    .get().url(strURL).id(100)
                    .tag(m_context).build()
                    .execute(new DownLoadFileCallBack(strPath, strFile));
        }
    }

    private void ShowStockList () {
        int nStart = m_strResponse.indexOf('{');
        int nEnd = m_strResponse.lastIndexOf('}');
        String strResult = m_strResponse.substring(nStart, nEnd+1);
        JSONObject  jsnItem = null;

        int nSize = m_lstCode.size();
        m_strInfo = new String[nSize];
        m_nColor = new int[nSize];
        JSONObject  jsnOne = null;
        JSONObject  jsnResult = JSON.parseObject(strResult);
        for (int i = 0; i < nSize; i++) {
            jsnItem = jsnResult.getJSONObject(m_lstCode.get(i));
            String  strName     = jsnItem.getString("name");
            double  dNowPrice   = jsnItem.getDoubleValue("price");
            double  dYstPrice   = jsnItem.getDoubleValue("yestclose");
            double  dPercent    = jsnItem.getDoubleValue("percent") * 100;
            long    lTurnover   = jsnItem.getLong("turnover") / 100000;
            if (i == 0)
                lTurnover = lTurnover / 1000;

            m_strUpdateTime     = "更新时间  " + jsnItem.getString("update");
            if (dPercent < 0)
                dPercent = -dPercent;
            m_strInfo[i] = String.format("%s  %.2f  % .2f%%  %,d", strName, dNowPrice, dPercent, lTurnover);

            m_nColor[i] = dNowPrice > dYstPrice ? 1 : 0;

            if (m_strStockCode != null && m_strStockCode.compareTo(m_lstCode.get(i)) == 0) {
                jsnOne = jsnItem;
            }
        }
        m_lstStock.setAdapter(new stockListAdapter());
        if (jsnOne != null) {
            AddOneHist ();
            m_jsnItem = jsnOne;
            m_lstOne.setAdapter(new stockOneAdapter());
        }
    }

    private void ShowStockOne () {
        int nStart = m_strStockOne.indexOf('{');
        int nEnd = m_strStockOne.lastIndexOf('}');
        String strResult = m_strStockOne.substring(nStart, nEnd + 1);
        JSONObject jsnResult = JSON.parseObject(strResult);
        AddOneHist ();
        m_jsnItem = jsnResult.getJSONObject(m_strStockCode);
        m_lstOne.setAdapter(new stockOneAdapter());
    }

    private boolean AddOneHist () {
        if (m_jsnItem != null) {
            String  strTime     = m_jsnItem.getString("time");
            if (m_strPrevTime.compareTo((strTime)) != 0) {
                long lVolume = m_jsnItem.getLong("volume");
                String strHist = String.format("  %s %d", strTime.substring(14), (lVolume - m_lPrevvolume)/ 100);
                m_lstHistory.add (strHist);
                m_strPrevTime = strTime;
                m_lPrevvolume = lVolume;
                return true;
            }
        }
        return false;
    }
    private boolean OneInList (String strCode) {
        boolean bFound = false;
        for (int i = 0; i < m_lstCode.size(); i++) {
            if (m_lstCode.get(i).compareTo(strCode) == 0) {
                bFound = true;
                break;
            }
        }
        return bFound;
    }

    class msgHandler extends Handler {
        public void handleMessage(Message msg) {
             if (msg.what ==MSG_UPDATE_INFO) {
                 m_bMessage = false;
                 UpdateStockList();
             } else if (msg.what == MSG_PARSE_RESULT) {
                 ShowStockList();
                 if (m_bResume) {
                     m_bMessage = true;
                     m_msgHandler.sendEmptyMessageDelayed(MSG_UPDATE_INFO, 1000);
                 }
             } else if (msg.what == MSG_PARSE_ONE) {
                 ShowStockOne();
             } else if (msg.what ==  MSG_HIDE_INPUT){
                 InputMethodManager imm = (InputMethodManager) m_context.getSystemService(Context.INPUT_METHOD_SERVICE);
                 imm.hideSoftInputFromWindow(m_edtStock.getWindowToken(), 0);
            }
        }
    }

    public class stockListAdapter extends BaseAdapter {
        public int getCount() {
            return m_lstCode.size() + 1;
        }
        public Object getItem(int arg0) {
            return arg0;
        }
        public long getItemId(int position) {
            return position;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            TextView mTextView = new TextView(getApplicationContext());
            if (position <  m_lstCode.size()) {
                mTextView.setText(m_strInfo[position]);
                if (m_nColor[position] > 0)
                    mTextView.setTextColor(m_nClrWhite);
                else
                    mTextView.setTextColor(m_nClrBlack);
            } else {
                mTextView.setText(m_strUpdateTime);
                mTextView.setTextColor(m_nClrWhite);
            }
            mTextView.setTextSize(24);
            return mTextView;
        }
    }

    public class stockOneAdapter extends BaseAdapter {
        public int getCount() {
            return 20;
        }
        public Object getItem(int arg0) {
            return arg0;
        }
        public long getItemId(int position) {
            return position;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            TextView mTextView = new TextView(getApplicationContext());
            if (m_jsnItem == null)
                return mTextView;
            double  dClose = m_jsnItem.getDoubleValue("yestclose");
            double  dPrice = 0;
            long    lNumber = 0;
            if (position == 0) {
                dPrice  = m_jsnItem.getDoubleValue("ask5");
                lNumber = m_jsnItem.getLong("askvol5");
            } else if (position == 1) {
                dPrice  = m_jsnItem.getDoubleValue("ask4");
                lNumber = m_jsnItem.getLong("askvol4");
            } else if (position == 2) {
                dPrice  = m_jsnItem.getDoubleValue("ask3");
                lNumber = m_jsnItem.getLong("askvol3");
            } else if (position == 3) {
                dPrice  = m_jsnItem.getDoubleValue("ask2");
                lNumber = m_jsnItem.getLong("askvol2");
            } else if (position == 4) {
                dPrice  = m_jsnItem.getDoubleValue("ask1");
                lNumber = m_jsnItem.getLong("askvol1");
            } else if (position == 5) {
                dPrice  = m_jsnItem.getDoubleValue("price");
            } else if (position == 6) {
                dPrice  = m_jsnItem.getDoubleValue("bid1");
                lNumber = m_jsnItem.getLong("bidvol1");
            } else if (position == 7) {
                dPrice  = m_jsnItem.getDoubleValue("bid2");
                lNumber = m_jsnItem.getLong("bidvol2");
            } else if (position == 8) {
                dPrice  = m_jsnItem.getDoubleValue("bid3");
                lNumber = m_jsnItem.getLong("bidvol3");
            } else if (position == 9) {
                dPrice  = m_jsnItem.getDoubleValue("bid4");
                lNumber = m_jsnItem.getLong("bidvol4");
            } else if (position == 10) {
                dPrice  = m_jsnItem.getDoubleValue("bid5");
                lNumber = m_jsnItem.getLong("bidvol5");
            }

            lNumber = lNumber / 100;
            String strItem = "";
            if (position == 5) {
                strItem = String.format("    %.2f", dPrice);
                mTextView.setTextSize(30);
            } else if (position <= 10){
                strItem = String.format("  %.2f  %,d", dPrice, lNumber);
                mTextView.setTextSize(18);
            } else {
                dClose = 0;
                int nIndex = position - 11;
                int nSize = m_lstHistory.size();
                if (nIndex < nSize) {
                    nIndex = nSize - nIndex - 1;
                    strItem = m_lstHistory.get(nIndex);
                }
                mTextView.setTextSize(18);
            }
            mTextView.setText(strItem);

            if (dPrice >= dClose)
                mTextView.setTextColor(m_nClrWhite);
            else
                mTextView.setTextColor(m_nClrBlack);
            return mTextView;
        }
    }

    public class httpDataCallBack extends StringCallback {
        public void onError(Call call, Exception e, int id) {
            Toast.makeText(m_context, "获取数据出错了！ " + e.getMessage(), Toast.LENGTH_SHORT).show();
            m_msgHandler.sendEmptyMessageDelayed(MSG_UPDATE_INFO, 2000);
        }

        public void onResponse(String response, int id) {
            if (id == 101) {
                m_strResponse =response;
                m_msgHandler.sendEmptyMessageDelayed(MSG_PARSE_RESULT, 0);
            } else {
                m_strStockOne = response;
                m_msgHandler.sendEmptyMessageDelayed(MSG_PARSE_ONE, 0);
            }
        }
    }

    public class DownLoadFileCallBack extends FileCallBack  {
        public DownLoadFileCallBack(String destFileDir,
                                    String destFileName) {
            super(destFileDir, destFileName);
            File file = new File(destFileDir);
            file.mkdir();
        }
        public void onBefore(Request request, int id) {
        }
        public void onAfter(int id) {
        }
        public void inProgress(float progress, long total, int id) {
        }
        public void onError(Call call, Exception e, int id) {
            //Toast.makeText(m_context, "下载文件出错了！ " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
        public void onResponse(File file, int id) {
             try {
                FileInputStream fis = new FileInputStream (file.getPath());
                Bitmap bmp = BitmapFactory.decodeStream(fis);
                m_imgStock.setImageBitmap(bmp);
                fis.close();
                file.delete();
             }catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private boolean CheckWritePermission (boolean bInit) {
        if (bInit) {
            //检查权限（NEED_PERMISSION）是否被授权 PackageManager.PERMISSION_GRANTED表示同意授权
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    != PackageManager.PERMISSION_GRANTED) {
                //用户已经拒绝过一次，再次弹出权限申请对话框需要给用户一个解释
                if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                    Toast.makeText(this, "请开通文件读写权限，否则无法正常使用本应用！", Toast.LENGTH_SHORT).show();
                }
                //申请权限
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQUEST_WRITE_EXTERNAL_STORAGE);

            } else {
                //Toast.makeText(this, "授权成功！", Toast.LENGTH_SHORT).show();
                return true;
            }
        } else {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                //Toast.makeText(this, "请开通文件读写权限，否则无法正常使用本应用！", Toast.LENGTH_SHORT).show();
                return false;
            }
        }
        return true;
    }
}
