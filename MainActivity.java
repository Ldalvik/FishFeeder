package root.fishfeeder;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import com.crashlytics.android.Crashlytics;
import io.fabric.sdk.android.Fabric;

public class MainActivity extends AppCompatActivity {
    EditText feedTime;
    Button dailyFeedTime, feedNow;
    Utils utils;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Fabric.with(this, new Crashlytics());
        setContentView(R.layout.activity_main);

        utils = new Utils(this);
        feedTime = (EditText) findViewById(R.id.feed_time);
        dailyFeedTime = (Button) findViewById(R.id.daily_feed_time);
        feedNow = (Button) findViewById(R.id.feed_now);

        dailyFeedTime.setEnabled(false);
        feedNow.setEnabled(false);

        utils.ButtonStatus(feedTime, dailyFeedTime, feedNow);
    }

    public void dailyFeedTime(View v) {
        utils.CalenderDialog(feedTime);
    }

    public void feedNow(View v) {
        String time = feedTime.getText().toString();
        utils.Send("feed=" + time);
    }

    public void checkFeedTime(View v) {
        utils.CheckFeedTime();
    }

    public void info(View v) {

    }
}
