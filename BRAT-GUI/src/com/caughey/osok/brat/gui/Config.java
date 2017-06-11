package com.caughey.osok.brat.gui;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Properties;

public class Config {
	
	private static Logger logger = new Logger(Config.class.getName());

	private static Config instance = null;
	private static String filename = "config.properties";
	
	private static final String BRAT_IP = "BRAT_IP";
	private static final String LOG_LEVEL = "LOG_LEVEL";
	private static final String PROXMARK_CLIENT_LOC = "PROXMARK_CLIENT_LOC";
	private static final String PROXMARK_DEVICE = "PROXMARK_DEVICE";
	
	private Properties props = null;
	
	public static Config getInstance(){
		if(instance == null){
			setInstance();
		}
		return instance;
	}
	
	private static synchronized void setInstance(){
		if(instance==null){
			instance = new Config();
			instance.init();
		}
	}
	
	
	private void init(){
	
		props = new Properties();
		File file = new File(filename);
		
		if(file.exists() && !file.isDirectory()){
			try {
				props.load(new FileInputStream(file));
			} catch (IOException e) {
				logger.info("Configuration file could not be loaded");
				e.printStackTrace();
			}
		}else{
			props.put(BRAT_IP, "192.168.4.1");
			props.put(LOG_LEVEL, "debug");
			props.put(PROXMARK_CLIENT_LOC, "./proxmark3/client/proxmark3");
			props.put(PROXMARK_DEVICE, "/dev/ttyACM0");
			
						
			save();
		}
	}
	
	public void save(){
		File file = new File(filename);
		try {
			props.store(new FileOutputStream(file), "Configuration for BRAT-GUI");
		} catch (IOException e) {
			logger.warn("Configuration file could not be saved.");
			e.printStackTrace();
		}
	}
	public void setProxmarkClientLoc(String val){
		props.replace(PROXMARK_CLIENT_LOC, val);
	}
	public void setProxmarkDevice(String val){
		props.replace(PROXMARK_DEVICE, val);
	}
	public void setBRAT_IP(String val){
		props.replace(BRAT_IP, val);
	}
	public void setLogLevel(String val){
		props.replace(LOG_LEVEL, val);
	}
	public String getProxmarkDevice(){
		return props.getProperty(PROXMARK_DEVICE);
	}
	public String getProxmarkClientLoc(){
		return props.getProperty(PROXMARK_CLIENT_LOC);
	}
	public String getBRAT_IP(){
		return props.getProperty(BRAT_IP);
	}
	public String getLogLevel(){
		return props.getProperty(LOG_LEVEL);
	}
	public boolean logLevelDebug(){
		return props.getProperty(LOG_LEVEL).equalsIgnoreCase("DEBUG");
	}
	public boolean logLevelInfo(){
		return props.getProperty(LOG_LEVEL).equalsIgnoreCase("INFO");
	}
	public boolean logLevelWarn(){
		return props.getProperty(LOG_LEVEL).equalsIgnoreCase("WARN");
	}
	public boolean logLevelError(){
		return props.getProperty(LOG_LEVEL).equalsIgnoreCase("ERROR");
	}
}
