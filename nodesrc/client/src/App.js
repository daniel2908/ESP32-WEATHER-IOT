import React from "react";
import './App.css';

function App() {
  const [data, setData] = React.useState([]);

  React.useEffect(() => {
    fetch("http://localhost:3001/weather/get")
      .then((res) => res.json())
      .then((data) => setData(data));
  }, [data]);

  // React.useEffect(() => {
  //   fetch("/api")
  //     .then((res) => res.json())
  //     .then((data) => setData(data));
  // }, []);
console.log(data)
  return (
    <div className="App">
      <header className="App-header">
        <p>
          {data.temp}°C <br></br>
          {data.humidity}%
        </p>
      </header>
    </div>
  );
}

export default App;
