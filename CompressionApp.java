import javax.swing.*;
import java.awt.event.*;
import java.io.*;

public class CompressionApp extends JFrame {
    private JTextField inputPath, outputPath;
    private JButton browseInput, browseOutput, compressButton, decompressButton;

    public CompressionApp() {
        setTitle("DATA SQUEEZE");
        setSize(500, 250);
        setLayout(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        JLabel inputLabel = new JLabel("Input File:");
        inputLabel.setBounds(30, 30, 100, 25);
        add(inputLabel);

        inputPath = new JTextField();
        inputPath.setBounds(120, 30, 200, 25);
        add(inputPath);

        browseInput = new JButton("Browse");
        browseInput.setBounds(330, 30, 100, 25);
        add(browseInput);

        JLabel outputLabel = new JLabel("Output File:");
        outputLabel.setBounds(30, 70, 100, 25);
        add(outputLabel);

        outputPath = new JTextField();
        outputPath.setBounds(120, 70, 200, 25);
        add(outputPath);

        browseOutput = new JButton("Browse");
        browseOutput.setBounds(330, 70, 100, 25);
        add(browseOutput);

        compressButton = new JButton("Compress");
        compressButton.setBounds(100, 130, 120, 30);
        add(compressButton);

        decompressButton = new JButton("Decompress");
        decompressButton.setBounds(240, 130, 120, 30);
        add(decompressButton);

        browseInput.addActionListener(e -> chooseFile(inputPath));
        browseOutput.addActionListener(e -> chooseFile(outputPath));

        compressButton.addActionListener(e -> runCppProgram(1));
        decompressButton.addActionListener(e -> runCppProgram(2));
    }

    private void chooseFile(JTextField field) {
        JFileChooser fileChooser = new JFileChooser();
        int option = fileChooser.showOpenDialog(this);
        if (option == JFileChooser.APPROVE_OPTION) {
            File file = fileChooser.getSelectedFile();
            field.setText(file.getAbsolutePath());
        }
    }

    private void runCppProgram(int mode) {
        String input = inputPath.getText();
        String output = outputPath.getText();

        if (input.isEmpty() || output.isEmpty()) {
            JOptionPane.showMessageDialog(this, "Please provide both input and output paths.");
            return;
        }

        try {
            // Assuming your C++ exe is named "compressor.exe" and located in the same directory
            String exeName = "compressor.exe";
            ProcessBuilder builder;

            if (mode == 1) {
                // Compress
                builder = new ProcessBuilder(exeName);
                builder.redirectInput(ProcessBuilder.Redirect.PIPE);
                Process process = builder.start();

                BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));
                writer.write("1\n" + input + "\n" + output + "\n");
                writer.flush();
                writer.close();

                process.waitFor();
                JOptionPane.showMessageDialog(this, "Compression finished!");

            } else {
                // Decompress
                builder = new ProcessBuilder(exeName);
                builder.redirectInput(ProcessBuilder.Redirect.PIPE);
                Process process = builder.start();

                BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));
                writer.write("2\n" + input + "\n" + output + "\n");
                writer.flush();
                writer.close();

                process.waitFor();
                JOptionPane.showMessageDialog(this, "Decompression finished!");
            }

        } catch (IOException | InterruptedException ex) {
            JOptionPane.showMessageDialog(this, "Error running C++ program: " + ex.getMessage());
        }
    }

    public static void main(String[] args) {
        new CompressionApp().setVisible(true);
    }
}
