export class Webcam {
  private static stream: MediaStream;

  private constructor() {}

  public static async getMediaStream(width: number): Promise<MediaStream> {
    if (Webcam.stream) {
      return Webcam.stream;
    }

    Webcam.stream = await navigator.mediaDevices.getUserMedia({
      video: {
        width: { exact: width },
      },
    });
    return Webcam.stream;
  }
}
