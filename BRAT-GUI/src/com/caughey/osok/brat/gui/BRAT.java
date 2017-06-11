package com.caughey.osok.brat.gui;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;

public class BRAT {
	private static Logger logger = new Logger(BRAT.class.getName());

	private ArrayList<String> badgeList = new ArrayList<>();

	private static final String USER_AGENT = "BRAT GUI/1.0";
	private static final String URI = "/hex";
	
	
	public void readList(){
		StringBuffer urlString = new StringBuffer();
		
		urlString.append("http://");
		urlString.append(Config.getInstance().getBRAT_IP());
		urlString.append(URI);
		
		try {
			URL url = new URL(urlString.toString());
			HttpURLConnection con = (HttpURLConnection) url.openConnection();
			con.setRequestMethod("GET");
			con.setRequestProperty("User-Agent", USER_AGENT);
			int responseCode = con.getResponseCode();
			logger.debug("Response Code : " + responseCode);
			if(responseCode == HttpURLConnection.HTTP_OK){
				BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
				String newBadge;
				StringBuffer response = new StringBuffer();

				while ((newBadge = in.readLine()) != null) {
					badgeList.add(newBadge);
				}
				in.close();				
			}
		} catch (MalformedURLException e) {
			logger.error("The URL to get the badges from the BRAT device is malformed.  Check the BRAT IP Address in the configuration.");
			e.printStackTrace();
		} catch (IOException e) {
			logger.warn("Socket connection to BRAT Device had some difficulties communiating");
			e.printStackTrace();
		}
	}
	
	public String[] getList(){
		String[] list = new String[badgeList.size()];
		Iterator<String> itt = badgeList.iterator();
		int i=0;
		while(itt.hasNext()){
			list[i++] = itt.next();
		}
		return list;
	}
}
