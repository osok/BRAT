package com.caughey.osok.brat.gui;

import java.awt.EventQueue;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.AbstractListModel;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.border.EmptyBorder;

public class BRATFrame extends JFrame {
	private static Logger logger = new Logger(BRATFrame.class.getName());
	
	private JPanel contentPane;
	private JList listBadges;
	private BRAT bratDevice = new BRAT();
	String[] badgeArray = new String[] {""};

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		
		 try {
	            // Set cross-platform Java L&F (also called "Metal")
	        UIManager.setLookAndFeel(
	            UIManager.getCrossPlatformLookAndFeelClassName());
	    } 
	    catch (UnsupportedLookAndFeelException e) {
	       // handle exception
	    }
	    catch (ClassNotFoundException e) {
	       // handle exception
	    }
	    catch (InstantiationException e) {
	       // handle exception
	    }
	    catch (IllegalAccessException e) {
	       // handle exception
	    }
		 
		
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					BRATFrame frame = new BRATFrame();
					frame.setVisible(true);
				} catch (Exception e) { 
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the frame.
	 */
	public BRATFrame() {
		setTitle("BRAT GUI");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 288, 341);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		contentPane.setLayout(null);
		
		JButton btnNewButton = new JButton("Write Badge");
		btnNewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String ss = null;
		        Runtime obj = null;
//		        try{
//		        	StringBuffer cmd = new StringBuffer();
//		        	// Proxmark3 client
//		        	cmd.append(Config.getInstance().getProxmarkClientLoc());
//		        	cmd.append(" ");
//		        	// Proxmark3 Device
//		        	cmd.append(Config.getInstance().getProxmarkDevice());
//			        logger.debug("Command : " + cmd.toString());
//
//			        Process p = Runtime.getRuntime().exec(cmd.toString());
//			        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(p.getOutputStream()));

		        
		        String hex = badgeArray[listBadges.getSelectedIndex()];
		        logger.debug("Hex : " + hex);
		        String script = "script run write_hid -x " + hex;
		        logger.debug("Script : " + script);
		        
		        try {
		        		ScriptCommandDialog dialog = new ScriptCommandDialog(script);
						dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
						dialog.setVisible(true);
					} catch (Exception ex) {
						ex.printStackTrace();
					}
			        
			        
			        
			        
//			        writer.write(script);
//			        logger.debug("flush... ");
//			        writer.flush();
//			        logger.debug("connect input... ");
//			        BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
//			        logger.debug("connect output... ");
//			        BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));
//			        logger.debug("done! ");
//		        } catch (IOException ioe) {
//					logger.error("Error occurred while writing the tag : " + ioe.getMessage());
//				}finally{
//		        	
//		        }
				
			}
		});
		btnNewButton.setBounds(10, 238, 250, 23);
		contentPane.add(btnNewButton);
		
		JScrollPane scrollPane = new JScrollPane();
		scrollPane.setBounds(10, 11, 250, 180);
		contentPane.add(scrollPane);
		
		listBadges = new JList();
		listBadges.setFont(new Font("Tahoma", Font.PLAIN, 14));
		listBadges.setModel(new AbstractListModel() {

			public int getSize() {
				return badgeArray.length;
			}
			public Object getElementAt(int index) {
				return badgeArray[index];
			}
		});
		listBadges.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		scrollPane.setViewportView(listBadges);
		
		JButton btnNewButton_1 = new JButton("Read from BRAT");
		btnNewButton_1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				bratDevice.readList();
				badgeArray = bratDevice.getList();
				listBadges.updateUI();
			}
		});
		btnNewButton_1.setBounds(10, 203, 250, 23);
		contentPane.add(btnNewButton_1);
		
		JButton btnConfigure = new JButton("Configure");
		btnConfigure.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				try {
					ConfigureDialog dialog = new ConfigureDialog();
					dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
					dialog.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
		btnConfigure.setBounds(10, 273, 250, 23);
		contentPane.add(btnConfigure);
	}
}
