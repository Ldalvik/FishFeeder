package root.fishfeeder;


public class Setup {
    private static final String DEVICE_ID = "2e001f000e47353136383631";
    private static final String FUNCTION = "fish_feeder";
    private static final String ACCESS_TOKEN = "13603d6d6d0dd75418d22bde371fc0afa4305273";

    public static String PHOTON_URL = Utils.Endpoint(DEVICE_ID, FUNCTION, ACCESS_TOKEN);
}
