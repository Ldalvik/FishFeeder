package root.fishfeeder;

import android.os.AsyncTask;
import android.os.Build;
import android.support.annotation.RequiresApi;
import java.io.DataOutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;

class Request extends AsyncTask<String, Void, String> {

    private String arg;

    Request(String arg) {
        this.arg = arg;
    }

    @Override
    protected void onPreExecute() {

    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    protected String doInBackground(String... params) {

        try {
            URL obj = new URL(Setup.PHOTON_URL);
            HttpURLConnection con = (HttpURLConnection) obj.openConnection();
            con.setRequestMethod("POST");

            String urlParameters = "arg=" + arg;
            byte[] postData = new byte[0];
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT) {
                postData = urlParameters.getBytes(StandardCharsets.UTF_8);
            }
            int postDataLength = postData.length;
            con.setDoOutput(true);
            con.setInstanceFollowRedirects(false);
            con.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
            con.setRequestProperty("charset", "utf-8");
            con.setRequestProperty("Content-Length", Integer.toString(postDataLength));
            con.setUseCaches(false);
            DataOutputStream wr = new DataOutputStream(con.getOutputStream());
            wr.write(postData);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return arg;
    }

    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);
        if (result != null) {
            System.out.println("Request sent->" + result);
        }
    }
}