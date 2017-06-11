package com.caughey.osok.brat.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;

public class ScriptCommandDialog extends JDialog {

	private final JPanel contentPanel = new JPanel();
	private JTextField textFieldScript;
	private String scriptCommand = "";

	/**
	 * Create the dialog.
	 */
	public ScriptCommandDialog(String scriptCommand) {
		this.scriptCommand = scriptCommand;
		setBounds(100, 100, 450, 139);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(null);
		{
			textFieldScript = new JTextField();
			textFieldScript.setEditable(false);
			textFieldScript.setBounds(10, 39, 316, 20);
			contentPanel.add(textFieldScript);
			textFieldScript.setColumns(10);
			textFieldScript.setText(scriptCommand);
			
		}
		{
			JLabel lblScriptCommand = new JLabel("Script Command");
			lblScriptCommand.setBounds(10, 11, 126, 14);
			contentPanel.add(lblScriptCommand);
		}
		{
			JButton btnNewButton = new JButton("Copy");
			btnNewButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					StringSelection stringSelection = new StringSelection(textFieldScript.getText());
					Clipboard clpbrd = Toolkit.getDefaultToolkit().getSystemClipboard();
					clpbrd.setContents(stringSelection, null);
				}
			});
			btnNewButton.setBounds(338, 38, 86, 21);
			contentPanel.add(btnNewButton);
		}
		{
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
			getContentPane().add(buttonPane, BorderLayout.SOUTH);
			{
				JButton okButton = new JButton("OK");
				okButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						dispose();
					}
				});
				okButton.setActionCommand("OK");
				buttonPane.add(okButton);
				getRootPane().setDefaultButton(okButton);
			}
		}
	}

}
