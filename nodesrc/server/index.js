const express = require("express");
const bodyParser = require("body-parser")


const PORT = process.env.PORT || 3001;

let data;
let cors = require('cors');



const app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json())
app.use(cors({origin: 'http://localhost:3000'}));

app.get("/weather/get", (req, res) => {
  res.send(data);
  console.log(data);
});

app.put('/weather', (req, res) => {
  res.type('json');
  
  // try{
  //   res.json({
  //     temp: req.body.temp,
  //     humidity: req.body.humidity
  //   })
  // }
  // catch (error){
  //   res.status(500).send({
  //   error: error
  //   })
  // }
  data = req.body;
  console.log(req.body);
  res.send("got it")
});

app.listen(PORT, () => {
  console.log(`Server listening on ${PORT}`);
});