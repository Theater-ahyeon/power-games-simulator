---
name: "minimax-multimodal-toolkit"
description: "Generate voice, music, video, and image content via MiniMax APIs. Invoke when user needs to create multimedia assets like images, videos, audio, or music."
---

# MiniMax Multimodal Toolkit

This skill provides access to MiniMax's multimodal capabilities for generating various types of media content.

## Features

- **Text-to-Speech (TTS)**: Convert text to natural-sounding speech
- **Voice Cloning**: Clone voices for consistent narration
- **Voice Design**: Create custom voices for different characters
- **Music Generation**: Generate songs and instrumentals
- **Video Generation**: Create videos from text or images
- **Image Generation**: Generate images from text descriptions
- **Image-to-Image**: Modify existing images based on text prompts
- **Media Processing**: Convert, concatenate, trim, and extract media files

## API Configuration

### Required Parameters
- **API Key**: Your MiniMax API key
- **Service Region**: Select between China mainland or Global service

### Configuration Example

```json
{
  "api_key": "sk-cp-xxxx",
  "service_region": "china_mainland"
}
```

## Usage Examples

### Generate an Image
```
Generate an image of a futuristic political war room with high-tech displays
```

### Generate a Song
```
Create an instrumental track that sounds like a dramatic political thriller soundtrack
```

### Generate Speech
```
Convert this text to speech: 'The fate of the nation hangs in the balance'
```

## Note

This skill requires a valid MiniMax API key to function. The API key provided by the user (sk-cp-JR_GuEdo49wRQox_Uba8MovfD0GkR-BFFQtHWhLe-rDrnlAniHYX4DrzLovaC_dc93FTBxguAqZdKH9cx2f3We5wPrOeaPUrYwiA89Ra63M3K4OsbfwoLq8) has been configured for use with China mainland service.
