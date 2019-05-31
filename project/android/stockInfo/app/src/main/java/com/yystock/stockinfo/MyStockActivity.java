package com.yystock.stockinfo;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.zhy.http.okhttp.OkHttpUtils;
import com.zhy.http.okhttp.callback.FileCallBack;
import com.zhy.http.okhttp.callback.StringCallback;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import okhttp3.Call;
import okhttp3.Request;


public class MyStockActivity extends AppCompatActivity {
    public static final int             MSG_UPDATE_INFO     = 1001;
    public static final int             MSG_PARSE_RESULT    = 1002;

    private ArrayList<stockItem>        m_lstStockInfo = null;
    private ArrayList<stockComp>        m_lstStockComp = null;

    private MyStockActivity.msgHandler  m_msgHandler    = null;

    private String                      m_strResponse = null;
    private ArrayList<stockView>        m_lstResult = null;

    private Context                     m_context = null;
    private ListView                    m_lvMyStock = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_stock);
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            //actionBar.hide();
            int color = Color.parseColor("#00574B");
            ColorDrawable drawable = new ColorDrawable(color);
            actionBar.setBackgroundDrawable(drawable);
        }
        super.setTitle("MyStock");

        readMyStockFile ();

        m_lvMyStock = (ListView)findViewById(R.id.lstMyStock);

        m_msgHandler = new MyStockActivity.msgHandler();
        m_msgHandler.sendEmptyMessageDelayed(MSG_UPDATE_INFO, 1000);
    }

    private void updateInfo () {
        if (m_lstStockInfo == null)
            return;

        String strURL = "https://api.money.126.net/data/feed/";
        for (int i = 0; i < m_lstStockInfo.size(); i++) {
            strURL += m_lstStockInfo.get(i).m_strCode; strURL += ",";
        }
        OkHttpUtils
                .get().url(strURL).id(101)
                .build().execute(new MyStockActivity.httpDataCallBack());
    }

    private void showResult () {
        if (m_lstStockInfo == null)
            return;

        m_lstResult.clear();

        int nStart = m_strResponse.indexOf('{');
        int nEnd = m_strResponse.lastIndexOf('}');
        String strResult = m_strResponse.substring(nStart, nEnd+1);

        int i, j;
        int nSize = m_lstStockInfo.size();
        JSONObject  jsnItem = null;
        stockItem   stkItem = null;
        stockView   stkView = null;
        JSONObject  jsnResult = JSON.parseObject(strResult);
        for (i = 0; i < nSize; i++) {
            stkItem = m_lstStockInfo.get(i);
            jsnItem = jsnResult.getJSONObject(stkItem.m_strCode);
            if (jsnItem != null) {
                stkItem.m_dNowPrice = jsnItem.getDoubleValue("price");
                stkItem.m_dClsPrice = jsnItem.getDoubleValue("yestclose");
                stkItem.m_strName = jsnItem.getString("name");
            }
        }

        String      strItem = "myStock";
        stockComp   stkComp = null;
        double      dCompValue = 0;
        double      dTotalStock = 0;
        double      dTotalCash = 0;
        double      dTotalDebt = 0;

        for (i = 0; i < m_lstStockComp.size(); i++) {
            stkComp = m_lstStockComp.get (i);
            dTotalCash = dTotalCash + stkComp.m_dCash;
            dTotalDebt = dTotalDebt + stkComp.m_dDebt;

            dCompValue = 0;
            dTotalStock = 0;
            for (j = 0; j < m_lstStockInfo.size(); j++) {
                stkItem = m_lstStockInfo.get(j);
                if (stkItem.m_strComp.compareTo(stkComp.m_strComp) == 0)
                    dCompValue = dCompValue + stkItem.m_nNumber * stkItem.m_dNowPrice;
                dTotalStock = dTotalStock + stkItem.m_nNumber * stkItem.m_dNowPrice;
            }
            stkView = new stockView ();
            stkView.m_strItem = String.format("%s  %,.2f  %,.2f  %,.2f", stkComp.m_strName, dCompValue, stkComp.m_dCash, stkComp.m_dDebt);
            stkView.m_nTextColor = Color.rgb(255, 255,255);
            stkView.m_nTextSize = 18;
            m_lstResult.add (stkView);

            for (j = 0; j < m_lstStockInfo.size(); j++) {
                stkItem = m_lstStockInfo.get(j);
                if (stkItem.m_strComp.compareTo(stkComp.m_strComp) != 0)
                    continue;
                stkView = new stockView ();
                stkView.m_strItem = String.format("%s  %,.2f  %,.2f%%  %,.2f  %,.2f", stkItem.m_strName, stkItem.m_dNowPrice * stkItem.m_nNumber,
                        ((stkItem.m_dNowPrice - stkItem.m_dBuyPrice) / stkItem.m_dBuyPrice) * 100, stkItem.m_dBuyPrice, stkItem.m_dNowPrice);
                stkView.m_nTextColor = Color.rgb(168, 168,168);
                stkView.m_nTextSize = 18;
                m_lstResult.add (stkView);
            }

            m_lstResult.add (new stockView ());
        }

        ArrayList<stockItem>        lstItems = new ArrayList<stockItem>();
        stockItem                   stkToday = null;

        strItem = "  ";
        int     nTotalNum = 0;
        double  dTotalBuy = 0;
        double  dTotalNow = 0;
        double  dTotalWin = 0;

        String  strName = "";
        for (j = 0; j < m_lstStockInfo.size(); j++) {
            stkItem = m_lstStockInfo.get(j);
            dTotalWin += stkItem.m_nNumber * (stkItem.m_dNowPrice - stkItem.m_dClsPrice);
            if (strItem.compareTo(stkItem.m_strCode) == 0) {
                nTotalNum += stkItem.m_nNumber;
                dTotalBuy += stkItem.m_nNumber * stkItem.m_dBuyPrice;
                dTotalNow += stkItem.m_nNumber * stkItem.m_dNowPrice;
                strName = stkItem.m_strName;

                stkToday.m_nNumber += stkItem.m_nNumber;

            } else {
                if (nTotalNum > 0) {
                    stkView = new stockView ();
                    stkView.m_strItem = String.format("%s  %,.2f  %,.2f  %,.2f%%", strName,
                            dTotalNow, dTotalNow - dTotalBuy, ((dTotalNow - dTotalBuy) * 100) / dTotalBuy);
                    stkView.m_nTextSize = 18;
                    if (dTotalNow > dTotalBuy)
                        stkView.m_nTextColor = Color.rgb(255, 255,255);
                    else
                        stkView.m_nTextColor = Color.rgb(168, 168,168);
                    m_lstResult.add (stkView);

                    lstItems.add(stkToday);
                    stkToday = null;
                }

                strItem = stkItem.m_strCode;
                nTotalNum = stkItem.m_nNumber;
                dTotalBuy = stkItem.m_nNumber * stkItem.m_dBuyPrice;
                dTotalNow = stkItem.m_nNumber * stkItem.m_dNowPrice;
                strName = stkItem.m_strName;

                if (stkToday == null)
                    stkToday = new stockItem();
                stkToday.m_strName = strName;
                stkToday.m_nNumber += stkItem.m_nNumber;
                stkToday.m_dClsPrice = stkItem.m_dClsPrice;
                stkToday.m_dNowPrice = stkItem.m_dNowPrice;
            }

        }
        stkView = new stockView ();
        stkView.m_strItem = String.format("%s  %,.2f  %,.2f  %,.2f%%", strName,
                dTotalNow, dTotalNow - dTotalBuy, ((dTotalNow - dTotalBuy) * 100) / dTotalBuy);
        if (dTotalNow > dTotalBuy)
            stkView.m_nTextColor = Color.rgb(255, 255,255);
        else
            stkView.m_nTextColor = Color.rgb(168, 168,168);
        stkView.m_nTextSize = 18;
        m_lstResult.add (stkView);
        lstItems.add(stkToday);

        m_lstResult.add (new stockView ());
        for (i = 0; i < lstItems.size(); i++) {
            stkItem = lstItems.get(i);
            stkView = new stockView ();
            double dPercent = (stkItem.m_dNowPrice - stkItem.m_dClsPrice) * 100 / stkItem.m_dClsPrice;
            double dStockWin = stkItem.m_nNumber * (stkItem.m_dNowPrice - stkItem.m_dClsPrice);
            stkView.m_strItem = String.format("%s  %d  %,.2f  %,.2f%%,   %,.2f", stkItem.m_strName,
                    stkItem.m_nNumber, stkItem.m_dNowPrice, dPercent, dStockWin);
            stkView.m_nTextSize = 18;
            if (stkItem.m_dNowPrice > stkItem.m_dClsPrice)
                stkView.m_nTextColor = Color.rgb(255, 255,255);
            else
                stkView.m_nTextColor = Color.rgb(168, 168,168);
            m_lstResult.add (stkView);
        }
        lstItems.clear();

        m_lstResult.add (new stockView ());
        stkView = new stockView ();
        stkView.m_strItem = String.format("汇总：  %,.2f  %,.2f  %,.2f", dTotalStock, dTotalCash, dTotalDebt);
        stkView.m_nTextSize = 18;
        m_lstResult.add (stkView);
        stkView = new stockView ();
        stkView.m_strItem = String.format("资产：  %,.2f      %,.2f", dTotalStock + dTotalCash - dTotalDebt, dTotalWin);
        stkView.m_nTextSize = 18;
        m_lstResult.add (stkView);

        m_lvMyStock.setAdapter(new myStockAdapter());
    }

    public class myStockAdapter extends BaseAdapter {
        public int getCount() {
            return m_lstResult.size();
        }
        public Object getItem(int arg0) {
            return arg0;
        }
        public long getItemId(int position) {
            return position;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            TextView mTextView = new TextView(getApplicationContext());
            mTextView.setText(m_lstResult.get(position).m_strItem);
            mTextView.setTextColor(m_lstResult.get(position).m_nTextColor);
            mTextView.setTextSize(m_lstResult.get(position).m_nTextSize);
            return mTextView;
        }
    }

    private void readMyStockFile () {
        String strMyStockFile = "/sdcard/yyStock/mystock.txt";
        File fileMyStock = new File (strMyStockFile);
        if (!fileMyStock.exists()){
            return;
        }
        m_lstStockInfo = new ArrayList<stockItem>();
        m_lstStockComp = new ArrayList<stockComp>();
        m_lstResult = new ArrayList<stockView>();
        int     nNextPos = 0;
        String  strItem = "";
        String  strComp = "";
        try {
            FileInputStream fis = new FileInputStream ("/sdcard/yyStock/mystock.txt");
            BufferedReader br = new BufferedReader(new InputStreamReader(fis));
            String strLine = null;
            while((strLine = br.readLine())!=null) {
                if (strLine.substring(0, 1).compareTo(":") == 0) {
                    stockComp comp = new stockComp ();
                    m_lstStockComp.add(comp);
                    nNextPos = strLine.indexOf(':', 2);
                    strItem = strLine.substring(1, nNextPos);
                    comp.m_strComp = strItem;
                    strComp = strItem;
                    if (strItem.compareTo("DXZQ") == 0)
                        comp.m_strName = "东兴证券";
                    else if (strItem.compareTo("ZSZQ") == 0)
                        comp.m_strName = "招商证券";
                    else if (strItem.compareTo("ZSRZ") == 0)
                        comp.m_strName = "招商融资";

                    nNextPos = strLine.indexOf('=', 2);
                    strLine = strLine.substring(nNextPos + 1);

                    nNextPos = strLine.indexOf(':', 0);
                    if (nNextPos < 0) {
                        comp.m_dCash = Double.parseDouble(strLine);
                    } else{
                        strItem = strLine.substring(0, nNextPos);
                        comp.m_dCash = Double.parseDouble(strItem);

                        nNextPos = strLine.indexOf('=', 0);
                        strLine = strLine.substring(nNextPos + 1);
                        comp.m_dDebt = Double.parseDouble(strLine);
                    }

                } else {
                    stockItem item = new stockItem();
                    m_lstStockInfo.add(item);

                    nNextPos = strLine.indexOf(',', 2);
                    item.m_strCode = strLine.substring(0, nNextPos);
                    if (item.m_strCode.length() == 6) {
                        if (item.m_strCode.charAt(0) == '6')
                            item.m_strCode = "0" + item.m_strCode;
                        else if (item.m_strCode.charAt(0) == '3')
                            item.m_strCode = "1" + item.m_strCode;
                        else if (item.m_strCode.charAt(0) == '0' && item.m_strCode.charAt(1) == '0')
                            item.m_strCode = "1" + item.m_strCode;
                    }

                    strLine = strLine.substring(nNextPos + 1);
                    nNextPos = strLine.indexOf(',', 2);
                    item.m_strTime = strLine.substring(0, nNextPos);

                    strLine = strLine.substring(nNextPos + 1);
                    nNextPos = strLine.indexOf(',', 2);
                    item.m_dBuyPrice = Double.parseDouble(strLine.substring(0, nNextPos));

                    strLine = strLine.substring(nNextPos + 1);
                    item.m_nNumber = Integer.parseInt(strLine);

                    item.m_strComp = strComp;
                }

            }
        }catch (Exception e) {
            e.printStackTrace();
        }

        SortComparator sortComp = new SortComparator ();
        Collections.sort (m_lstStockInfo, sortComp);
    }

    public class SortComparator implements Comparator {
        public int compare(Object lhs, Object rhs) {
            stockItem a = (stockItem) lhs;
            stockItem b = (stockItem) rhs;
            return (a.m_strCode.compareTo(b.m_strCode));
        }
    }

    public class stockItem {
        public  String      m_strComp = null;
        public  String      m_strCode = null;
        public  String      m_strName = null;
        public  String      m_strTime = null;
        public  double      m_dBuyPrice = 0;
        public  int         m_nNumber = 0;
        public  double      m_dNowPrice = 0;
        public  double      m_dClsPrice = 0;
    }
    public class stockComp {
        public  String      m_strComp = null;
        public  String      m_strName = null;
        public  double      m_dCash = 0;
        public  double      m_dDebt = 0;
    }
    public class stockView {
        public  String      m_strItem = "  ";
        public  int         m_nTextSize = 8;
        public  int         m_nTextColor = Color.rgb(255, 255,255);
    }

    class msgHandler extends Handler {
        public void handleMessage(Message msg) {
            if (msg.what ==MSG_UPDATE_INFO) {
                updateInfo ();
            } else if (msg.what == MSG_PARSE_RESULT) {
                showResult();
                m_msgHandler.sendEmptyMessageDelayed(MSG_UPDATE_INFO, 1000);
            }
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
            }
        }
    }

}
