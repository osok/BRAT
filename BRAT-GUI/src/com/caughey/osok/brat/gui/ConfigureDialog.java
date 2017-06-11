package com.caughey.osok.brat.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.ComboBoxModel;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;

public class ConfigureDialog extends JDialog {

	private static Logger logger = new Logger(ConfigureDialog.class.getName());

	private final JPanel contentPanel = new JPanel();
	private JTextField textFieldBRAT_IP;
	private JTextField textFieldProxmarkClientLoc;
	private JTextField textFieldProxmarkDevice;
	
	/**
	 * Create the dialog.
	 */
	public ConfigureDialog() {
		
		
		setTitle("Configuration");
		setBounds(100, 100, 328, 276);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(null);
		
		JLabel lblNewLabel = new JLabel("BRAT IP Address");
		lblNewLabel.setBounds(10, 11, 131, 14);
		contentPanel.add(lblNewLabel);
		
		textFieldBRAT_IP = new JTextField();
		textFieldBRAT_IP.setBounds(118, 8, 184, 20);
		contentPanel.add(textFieldBRAT_IP);
		textFieldBRAT_IP.setColumns(10);
		
		JLabel lblDebugLevel = new JLabel("debug level");
		lblDebugLevel.setBounds(10, 176, 103, 14);
		contentPanel.add(lblDebugLevel);
		
		JComboBox comboBox = new JComboBox();
		comboBox.setModel(new DefaultComboBoxModel(new String[] {"Debug", "Info", "Warn", "Error"}));
		comboBox.setMaximumRowCount(4);
		comboBox.setBounds(118, 173, 88, 20);
		contentPanel.add(comboBox);
		{
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
			getContentPane().add(buttonPane, BorderLayout.SOUTH);
			{
				JButton saveButton = new JButton("Save");
				saveButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						Config c = Config.getInstance();
						c.setBRAT_IP(textFieldBRAT_IP.getText());
						c.setLogLevel((String)comboBox.getSelectedItem());
//						c.setProxmarkClientLoc(textFieldProxmarkClientLoc.getText());
//						c.setProxmarkDevice(textFieldProxmarkDevice.getText());
						c.save();
						dispose();
					}
				});
				saveButton.setActionCommand("OK");
				buttonPane.add(saveButton);
				getRootPane().setDefaultButton(saveButton);
			}
			{
				JButton cancelButton = new JButton("Cancel");
				cancelButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						dispose();
					}
				});
				cancelButton.setActionCommand("Cancel");
				buttonPane.add(cancelButton);
			}
		}
		
		
		textFieldBRAT_IP.setText(Config.getInstance().getBRAT_IP());
		
//		JLabel lblProxmarkClientDirectory = new JLabel("Proxmark Client Directory");
//		lblProxmarkClientDirectory.setBounds(10, 40, 219, 14);
//		contentPanel.add(lblProxmarkClientDirectory);
//		
//		textFieldProxmarkClientLoc = new JTextField();
//		textFieldProxmarkClientLoc.setBounds(10, 65, 254, 20);
//		contentPanel.add(textFieldProxmarkClientLoc);
//		textFieldProxmarkClientLoc.setColumns(10);
//		textFieldProxmarkClientLoc.setText(Config.getInstance().getProxmarkClientLoc());
//		
//		JButton buttonSelectFile = new JButton("...");
//		buttonSelectFile.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent e) {
//				final JFileChooser fc = new JFileChooser();
//				int returnVal = fc.showOpenDialog(ConfigureDialog.this);
//				if (returnVal == JFileChooser.APPROVE_OPTION) {
//					File proxmarkClient = fc.getSelectedFile();
//					Config.getInstance().setProxmarkClientLoc(proxmarkClient.getAbsolutePath());
//					textFieldProxmarkClientLoc.setText(Config.getInstance().getProxmarkClientLoc());
//		            //This is where a real application would open the file.
//		            logger.info("Opening: " + proxmarkClient.getName() );
//		        } else {
//		        	logger.info("Open command cancelled by user.");
//		        }
//			}
//		});
//		buttonSelectFile.setBounds(274, 64, 28, 23);
//		contentPanel.add(buttonSelectFile);
//		
//		JLabel lblProxmarkDevice = new JLabel("Proxmark3 Device");
//		lblProxmarkDevice.setBounds(10, 100, 115, 14);
//		contentPanel.add(lblProxmarkDevice);
//		
//		textFieldProxmarkDevice = new JTextField();
//		textFieldProxmarkDevice.setBounds(118, 96, 184, 20);
//		contentPanel.add(textFieldProxmarkDevice);
//		textFieldProxmarkDevice.setColumns(10);
//		textFieldProxmarkDevice.setText(Config.getInstance().getProxmarkDevice());
		
		ComboBoxModel model = comboBox.getModel();
		String item;
		for(int i=0, len = model.getSize(); i < len; i++){
			item = (String)model.getElementAt(i);
			if(item.equalsIgnoreCase(Config.getInstance().getLogLevel())){
				model.setSelectedItem(item);
			}
		}
	}
}
