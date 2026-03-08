// "use client";

import {
  Card,
  Table,
  TableHeader,
  TableColumn,
  TableBody,
  TableRow,
  TableCell,
  Divider,
} from "@nextui-org/react";
import { useEffect } from "react";

export default function CardStaWebStart() {
  function handleStartStaWeb() {
    console.log("start sta web server");
    fetch("/api/v1/start_sta_web")
      .then((response) => response.json())
      .then((data) => {
        console.log(data);
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
        }
      });
  }

  useEffect(() => {
    handleStartStaWeb();
  }, []);

  return (
    <Card className="grow">
      <div className="mx-10 mt-5 mb-5 flex gap-x-5 justify-end items-center">
        <p className="grow mr-5 text-3xl font-serif font-bold text-center">
          Notice
        </p>
      </div>
      <div className="mx-5 mt-2">
        <p className="text-center">
          UnitCamS3 will reboot and start in Web Server (STA) mode.
        </p>

        <Divider className="mt-5 mb-10"></Divider>

        <p className="my-5 text-center">
          The <span className="font-bold">LED Status</span> shows the current
          state:
        </p>
        <Table aria-label="Mode Table" className="my-5">
          <TableHeader>
            <TableColumn>LED STATUS</TableColumn>
            <TableColumn>STATE</TableColumn>
          </TableHeader>
          <TableBody>
            <TableRow key="1">
              <TableCell>Constantly on</TableCell>
              <TableCell>AP active (waiting for client)</TableCell>
            </TableRow>
            <TableRow key="2">
              <TableCell>Blinking</TableCell>
              <TableCell>10s AP wait countdown</TableCell>
            </TableRow>
            <TableRow key="3">
              <TableCell>Breathing</TableCell>
              <TableCell>Connecting to WiFi (STA)</TableCell>
            </TableRow>
            <TableRow key="4">
              <TableCell>Constantly on</TableCell>
              <TableCell>Web server running (STA mode)</TableCell>
            </TableRow>
          </TableBody>
        </Table>

        <Divider className="mt-10 mb-5"></Divider>

        <p className="grow mr-5 text-3xl font-serif font-bold text-center">
          Access
        </p>

        <p className="mt-5 mb-5 text-center">
          After connecting to the configured WiFi, access the web server via the
          device&apos;s <span className="font-bold">local IP address</span> in
          your browser.
        </p>

        <Divider className="mt-10 mb-5"></Divider>

        <p className="grow mr-5 text-3xl font-serif font-bold text-center">
          Reset
        </p>

        <p className="mt-5 mb-5 text-center">
          On every reboot, the device starts{" "}
          <span className="font-bold">UnitCamS3-WiFi</span> AP for{" "}
          <span className="font-bold">10 seconds</span>. Connect during that
          window to reconfigure.
        </p>
      </div>
    </Card>
  );
}
