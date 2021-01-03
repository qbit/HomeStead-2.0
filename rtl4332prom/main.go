package main

import (
	"bufio"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"strings"
	"time"
)

// AcuTime is a representation of time from the accurite
type AcuTime struct {
	time.Time
}

var cTime = "2006-01-02 15:04:05"

// UnmarshalJSON .
func (ct *AcuTime) UnmarshalJSON(b []byte) (err error) {
	s := strings.Trim(string(b), "\"")
	if s == "null" {
		ct.Time = time.Time{}
		return
	}
	ct.Time, err = time.Parse(cTime, s)
	return
}

// MarshalJSON .
func (ct *AcuTime) MarshalJSON() ([]byte, error) {
	if ct.Time.UnixNano() == nilTime {
		return []byte("null"), nil
	}
	return []byte(fmt.Sprintf("\"%s\"", ct.Time.Format(cTime))), nil
}

var nilTime = (time.Time{}).UnixNano()

// IsSet checks if we have a real time
func (ct *AcuTime) IsSet() bool {
	return ct.UnixNano() != nilTime
}

/*
// UnmarshalJSON specifies our custom format for timestamps
func (t AcuTime) UnmarshalJSON(a []byte) error {
	// Mon Jan 2 15:04:05 MST 2006
	// 2018/02/15 11:25:27
	var err error
	s := strings.Replace(string(a), `"`, "", -1)
	t.Time, err = time.Parse(cTime, s)
	return err
}
*/

// Entry is a single json blob that is output by rtl_433
type Entry struct {
	Time           AcuTime `json:"time"`
	Model          string  `json:"model"`
	SensorID       int     `json:"sensor_id"`
	Channel        string  `json:"channel"`
	Sequence       int     `json:"sequence_num"`
	Battery        string  `json:"battery"`
	MessageTypeOne int
	MessageTypeTwo int
	MessageType    int     `json:"message_type"`
	WindSpeed      float64 `json:"wind_speed_mph"`
	WindDirDeg     float64 `json:"wind_dir_deg"`
	WindDir        string  `json:"wind_dir"`
	Rain           float64 `json:"rainfall_accumulation_inch"`
	TempF          float64 `json:"temperature_F"`
	Humidity       int64   `json:"humidity"`
}

func main() {
	name := flag.String("name", "Outside", "Name of the station.")

	flag.Parse()

	scanner := bufio.NewScanner(os.Stdin)
	var m Entry
	m.MessageTypeOne = 0
	m.MessageTypeTwo = 0

	s := &http.Server{
		Addr: ":8811",
		Handler: http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			w.Write([]byte("# HELP wstation_humidity Last sampled humidity of weather station\n"))
			w.Write([]byte("# TYPE wstation_humidity gauge\n"))
			w.Write([]byte(fmt.Sprintf("wstation_humidity{name=\"%s\"} %d\n", *name, m.Humidity)))

			w.Write([]byte("# HELP wstation_temp_f Last sampled temperature in F of weather station\n"))
			w.Write([]byte("# TYPE wstation_temp_f gauge\n"))
			w.Write([]byte(fmt.Sprintf("wstation_temp_f{name=\"%s\"} %f\n", *name, m.TempF)))

			w.Write([]byte("# HELP wstation_temp_c Last sampled temperature in C of weather station\n"))
			w.Write([]byte("# TYPE wstation_temp_c gauge\n"))
			w.Write([]byte(fmt.Sprintf("wstation_temp_c{name=\"%s\"} %f\n", *name, (m.TempF-32)*5/9)))

			w.Write([]byte("# HELP wstation_wind_dir Last sampled wind direction weather station\n"))
			w.Write([]byte("# TYPE wstation_wind_dir gauge\n"))
			w.Write([]byte(fmt.Sprintf("wstation_wind_dir{name=\"%s\"} %f\n", *name, m.WindDirDeg)))

			w.Write([]byte("# HELP wstation_wind_speed Last sampled wind speed weather station\n"))
			w.Write([]byte("# TYPE wstation_wind_speed gauge\n"))
			w.Write([]byte(fmt.Sprintf("wstation_wind_speed{name=\"%s\"} %f\n", *name, m.WindSpeed)))

			w.Write([]byte("# HELP wstation_rain Last sampled rain fall for weather station\n"))
			w.Write([]byte("# TYPE wstation_rain gauge\n"))
			w.Write([]byte(fmt.Sprintf("wstation_rain{name=\"%s\"} %f\n", *name, m.Rain)))
		}),
	}

	go s.ListenAndServe()

	for scanner.Scan() {
		dec := json.NewDecoder(strings.NewReader(scanner.Text()))
		if err := dec.Decode(&m); err == io.EOF {
			break
		} else if err != nil {
			log.Println(err)
			break
			//log.Fatal(err)
		}
		if m.Sequence == 2 {
			if m.MessageTypeOne == 0 {
				m.MessageTypeOne = m.MessageType
			} else {
				if m.MessageTypeOne != 0 && m.MessageTypeTwo == 0 {
					m.MessageTypeOne = m.MessageType
					m.MessageTypeTwo = m.MessageType

					m.MessageTypeOne = 0
					m.MessageTypeTwo = 0
				}
			}
		}
		if err := scanner.Err(); err != nil {
			fmt.Fprintln(os.Stderr, "ohgod", err)
		}
	}
}
