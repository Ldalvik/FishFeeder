package root.fishfeeder;

public class FeedLog {
    private int id;
    private String log;
    private String date;

    public FeedLog(int id, String log, String date) {
        this.id = id;
        this.log = log;
        this.date = date;
    }

    public int getId() {
        return id;
    }

    public String getLog() {
        return log;
    }

    public String getDate() {
        return date;
    }
}
