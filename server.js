const express = require("express");
const multer = require("multer");
const { exec } = require("child_process");
const path = require("path");
const fs = require("fs");

const app = express();
const PORT = 3000;

// ✅ Serve frontend files
app.use(express.static(path.join(__dirname, "public")));

// ✅ Multer storage config
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, "uploads/"); // save in uploads folder
  },
  filename: function (req, file, cb) {
    const uniqueName = Date.now() + "-" + file.originalname;
    cb(null, uniqueName);
  }
});

const upload = multer({ storage: storage });

// ===================
//   Compress Route
// ===================
app.post("/upload", upload.single("file"), (req, res) => {
  if (!req.file) return res.status(400).send("No file uploaded.");

  const inputFile = path.resolve(req.file.path);
  const outputFile = inputFile + ".huf";

  const command = `"${path.resolve("compressor.exe")}" "${inputFile}" "${outputFile}"`;
  console.log("Running:", command);

  exec(command, (error, stdout, stderr) => {
    console.log("stdout:", stdout);
    console.log("stderr:", stderr);

    if (error) return res.status(500).send("Compression failed.");
    if (!fs.existsSync(outputFile)) return res.status(500).send("Compressed file not created.");

    res.download(outputFile, path.basename(outputFile));
  });
});

// ===================
//   Decompress Route
// ===================
app.post("/decompress", upload.single("file"), (req, res) => {
  if (!req.file) return res.status(400).send("No file uploaded.");

  const inputFile = path.resolve(req.file.path);
  const outputFile = inputFile + ".txt";

  const command = `"${path.resolve("decompressor.exe")}" "${inputFile}" "${outputFile}"`;
  console.log("Running:", command);

  exec(command, (error, stdout, stderr) => {
    console.log("stdout:", stdout);
    console.log("stderr:", stderr);

    if (error) return res.status(500).send("Decompression failed.");
    if (!fs.existsSync(outputFile)) return res.status(500).send("Output file not created.");

    res.download(outputFile, path.basename(outputFile));
  });
});

// ===================
app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
