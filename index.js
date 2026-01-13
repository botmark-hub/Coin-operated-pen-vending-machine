import "dotenv/config";
import express from "express";
import axios from "axios";

const app = express();
app.use(express.json());

const TOKEN = process.env.TOKEN;

app.post("/alert", async (req, res) => {
  const text = req.body.message || "มีการกดซื้อปากกา 1 ครั้ง 🖊️";  

  try {
    await axios.post(
      "https://api.line.me/v2/bot/message/broadcast",
      { messages: [{ type: "text", text }] },
      { headers: { Authorization: `Bearer ${TOKEN}` } }
    );

    res.json({ ok: true });
  } catch (e) {
    console.error(e?.response?.data || e);
    res.status(500).json({ ok: false });
  }
});

app.get("/", (req, res) => res.send("LINE BOT READY"));

app.listen(3000, () => console.log("Server ready on 3000"));
